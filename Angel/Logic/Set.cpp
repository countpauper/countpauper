#include "Logic/Set.h"
#include "Logic/Expression.h"
#include "Logic/Boolean.h"
#include <algorithm>
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

void Set::Add(Expression&& other)
{
    if (auto* association = other.GetIf<Association>())
    {
        items.emplace(association->Left(), association->Right());
    }
    else 
    {
        items.emplace(std::move(other), Boolean(true));
    }
}

const Expression* Set::Get(const Expression& e) const
{
    for(const auto& association: items)
    {
        if (association.first == e)
            return &association.second;
    }
    return nullptr;
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
    return size() == rhs.size() &&
        std::equal(begin(), end(),
                      rhs.begin());
}

Expression Set::Simplify() const
{
    Set simplified; 
    for(const auto item: items)
    {
        simplified.Add(Association(item.first.Simplify(), item.second.Simplify()).Simplify());
    }
    return simplified;   
}

Match Set::Matches(const Expression& e, const Variables& variables) const
{
    const Set* set = e.GetIf<Set>();
    if (!set)
        return Boolean(false);
        
    // TODO: implement, but how and what behavior 
    // {ginny, max} should match {max, ginny}
    // {ginny, $X, gizmo } should match {max, ginny, $Y} with $X = max & $Y = gizmo 
    // should be the same size at least, barring some sort of |Tail syntax 
    // so all items should be attempted to match with another and then that one should be out
    if (size()!=set->size())
        return Boolean(false); // TODO: tuple matching or whatever could happen instead.
    return Boolean(false);
}

Set Set::Substitute(const Variables& substitutions) const
{
    Set substitute; 
    for(const auto item: items)
    {
        substitute.Add(Association(item.first.Substitute(substitutions), item.second.Substitute(substitutions)).Simplify());
    }
    return substitute;   
}


Expression Set::Infer(const Knowledge& knowledge, const Variables& substitutions) const
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
        result.items.emplace(item.first.Infer(knowledge, substitutions),
            item.second.Infer(knowledge,substitutions));
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


Set::iterator::iterator(Inner::const_iterator i) :
    it(i)
{
}

Expression Set::iterator::operator*() const
{
    if (it->second == Boolean(true))
        return it->first;
    else
        return Association{Expression(it->first), Expression(it->second)};
}

Set::iterator& Set::iterator::operator++()
{
    it++;
    return *this;
}
Set::iterator Set::iterator::operator++(int)
{
    Set::iterator ret(it);
    ++it; 
    return ret;
}

bool Set::iterator::operator==(const Set::iterator& rhs) const
{
    return it == rhs.it;
}

Set::iterator Set::begin() const
{
    return iterator(items.begin());
}

Set::iterator Set::end() const
{
    return iterator(items.end());
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