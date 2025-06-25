#include "Logic/Set.h"
#include "Logic/Expression.h"
#include "Logic/Boolean.h"
#include "ContainerImpl.h"
#include <algorithm>
#include <numeric>
#include <iostream>

namespace Angel::Logic
{

Set::Set(std::initializer_list<Expression> setItems)
{  
    for(auto item: setItems)    // copy not great for performance
    {
        Add(std::move(item));
    }
}


Set::Set(std::vector<Expression>&& setItems)
{
    for(auto& item: setItems)
    {
        Add(std::move(item));
    }
}	
unsigned Set::Add(Expression&& other)
{
    return AddToContainer(std::move(other), [this](Expression&& item)
    {
        if (auto* association=item.GetIf<Association>())
        {
            auto result = items.emplace(association->Left(), association->Right());
            if (result.second)
                return 1;
            result.first->second = std::move(association->Right());
            return 0;
        }
        auto result = items.emplace(std::move(item), Boolean(true));
        return result.second?1:0;        
    });
}


unsigned Set::Remove(const Expression& e)
{
    if (auto* association = e.GetIf<Association>())
    {
        auto it = items.find(association->Left());
        if (it == items.end())
            return 0;
        if (it->second != association->Right())
            return 0;
        items.erase(it);
        return 1;
    }
    else 
    {
        return items.erase(e);
    }
}


Expression Set::Get(const Expression& e) const
{
    for(const auto& association: items)
    {
        if (association.first == e)
            return association.second;
    }
    return Boolean(false);
}

Expression Set::Pop(const Expression& e) 
{
    for(auto it = items.begin(); it!=items.end(); ++it)
    {
        if (it->first==e)
        {
            Expression result(std::move(it->second));
            items.erase(it);
            return std::move(result);
        }
    }
    return Expression();
}

std::size_t Set::size() const
{
    return items.size();
}

bool Set::empty() const
{
    return items.empty();
}

Set::operator bool() const
{
    return !items.empty();
}

bool Set::operator==(const Set& rhs) const
{
    if (size() != rhs.size())
        return false; 
    for(const auto& e:items)
    {
        if (rhs.Get(e.first) != e.second) 
            return false;
    }
    return true;
}

Set& Set::operator+=(const Set& rhs)
{
    for(auto item: rhs)
    {
        Add(std::move(item));
    }
    return *this;
}

Set& Set::operator-=(const Set& rhs)
{
    for(const auto& item: rhs)
    {
        Remove(item);
    }
    return *this;
}

Set& Set::operator&=(const Set& rhs)
{
    for(auto it = items.begin(); it!=items.end();)
    {
        auto rhsIt = rhs.items.find(it->first);
        if (rhsIt==rhs.items.end()) 
            it = items.erase(it);
        else if (it->second!=rhsIt->second)
            it = items.erase(it);
        else 
            ++it;
    }
    return *this;
}

Set& Set::operator|=(const Set& rhs)
{
    return operator+=(rhs);
}

Set operator+(Set lhs, const Set& rhs) 
{ 
    return lhs += rhs; 
}

Set operator-(Set lhs, const Set& rhs)
{
    return lhs -= rhs;
}

Set operator&(Set lhs, const Set& rhs)
{
    return lhs &= rhs;
}

Set operator|(Set lhs, const Set& rhs)
{
    return lhs |= rhs;
}


Set Set::Simplify() const
{
    Set simplified; 
    for(const auto item: items)
    {
        simplified.Add(Association(item.first.Simplify(), item.second.Simplify()).Simplify());
    }
    return simplified;   
}

Set Set::Assumptions() const
{
    return std::accumulate(begin(), end(), Set{}, [](const Set& assumptions, const Expression& item)
    {
        return assumptions + item.Assumptions();
    });    
}

Expression Set::MakeHypothesis(const Set& constants, bool reversed) const
{
    // then match variables in other to constants in this and move to hypothesis in reverse
    if (empty())
        return Boolean(constants.empty());
    if (size()>1)
        if (reversed)
            return Equal{constants, *this};
        else
            return Equal{*this, constants};
    assert(items.begin()->second == Boolean(true));
    const auto& var = items.begin()->first;
    if (const auto* singleVar = var.GetIf<Variable>()) 
    {
        if (constants.size()!=1)
            return Boolean(false);  // too many
        else 
            if (reversed)
                return Equal{*constants.begin(), var};
            else 
                return Equal{var, *constants.begin()};
    }
    else if (const auto* all = var.GetIf<All>())
    {
        if (auto allVar = all->GetVariable())
        {
            if (reversed)
                return Equal{constants, *allVar};
            else 
                return Equal{*allVar, constants};
        }
    }
    assert(false); // MakeHypothesis should only be called on a set of variable expressions
    return Boolean(false);
}


Expression Set::Matches(const Expression& e, const Hypothesis& hypothesis) const
{
    const Set* set = e.GetIf<Set>();
    if (!set)
    {
        return Boolean(false);  
    }   
    auto simplified = Substitute(hypothesis).Simplify();
    auto other = set->Substitute(hypothesis).Simplify();
    // first remove all duplicates (inclunding matching var)
    auto duplicates = simplified & other; 
    simplified -= duplicates;
    other -= duplicates;

    // then match variables to constants and move to hypothesis 
    Set simplifiedAssumptions = simplified.Assumptions();
    Set otherAssumptions = other.Assumptions();
    
    simplified -= simplifiedAssumptions;
    other -= otherAssumptions;

    Hypothesis newHypothesis;
    newHypothesis.push_back(simplifiedAssumptions.MakeHypothesis(other, false));
    newHypothesis.push_back(otherAssumptions.MakeHypothesis(simplified, true));
    return newHypothesis.Simplify();
}

Set Set::Substitute(const Hypothesis& hypothesis) const
{
    Set substitute; 
    for(const auto item: items)
    {
        substitute.Add(Association(item.first.Substitute(hypothesis), item.second.Substitute(hypothesis)).Simplify());
    }
    return substitute;   
}


Expression Set::Infer(const Knowledge& knowledge, const Hypothesis& hypothesis, Trace& trace) const
{
    Set result;
    for(const auto& item: items)
    {
        // NB right side computed get lost here if the first side is now duplicated 
        // eg {X+1: cheese, X*2:pickles} with X=1 becomes {2:cheese} (or pickles perhaps)
        // this might be what is desired or maybe some expression makes more sense 
        // {2: cheese | pickles}. 
        // For cases where these are sets, which are true 2: true|true is still 2:true 
        // for cases where these are clauses eg legs(X+1): max legs(X*2): ginny, 
        // this disjunction also seems to make sense legs(2): max | ginny, if we are 
        // hypothesizing that both have 2 legs 
        result.items.emplace(item.first.Infer(knowledge, hypothesis, trace),
            item.second.Infer(knowledge,hypothesis, trace));
    }
    return result;
}

std::size_t Set::Hash() const
{
    std::size_t result = std::numeric_limits<std::size_t>::max();
    std::hash<Expression> hasher;
    for(const auto& association: items)
    {
        result ^= hasher(association.first) ^ hasher(association.second);
    }
    return result;
}


Set::const_iterator::const_iterator(Inner::const_iterator i) :
    it(i)
{
}

Expression Set::const_iterator::operator*() const
{
    if (it->second == Boolean(true))
        return it->first;
    else
        return Association{Expression(it->first), Expression(it->second)};
}

Set::const_iterator& Set::const_iterator::operator++()
{
    it++;
    return *this;
}
Set::const_iterator Set::const_iterator::operator++(int)
{
    Set::const_iterator ret(it);
    ++it; 
    return ret;
}

bool Set::const_iterator::operator==(const Set::const_iterator& rhs) const
{
    return it == rhs.it;
}

Set::const_iterator Set::begin() const
{
    return const_iterator(items.begin());
}

Set::const_iterator Set::end() const
{
    return const_iterator(items.end());
}


std::ostream& operator<<(std::ostream& os, const Set& set)
{
    os << "{";
    bool first = true;
    for(const auto& node: set)
    {
        if (!first)
            os << ",";
        os << node;
        first = false;
    }

    os << "}";
    return os;
}

}