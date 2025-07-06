#include "Logic/Internal/Tuple.h"
#include "Logic/Expression.h"
#include "ContainerImpl.h"
#include <typeinfo>
#include <iostream>
#include <cassert>
#include <numeric>

namespace Angel::Logic
{

Tuple::Tuple(std::initializer_list<Expression> items) :
    std::vector<Expression>(items)
{
}

Tuple::Tuple(std::vector<Expression>&& items) :
    std::vector<Expression>(std::move(items))
{
}

Tuple::Tuple(Tuple_subrange items)
{
    reserve(std::ranges::size(items));
    for (auto&& e : items) {
        emplace_back(e);
    }    
}

Tuple::~Tuple()
{
}

bool Tuple::operator==(const Tuple& rhs) const
{
    if (size()!=rhs.size())
        return false;
    return std::equal(begin(), end(), rhs.begin());
}

Tuple& Tuple::operator+=(const Container& rhs)
{
    for(auto item: rhs)
    {
        auto back = cend();
        AddAt(back, std::move(item));
    }
    return *this;
}

Tuple& Tuple::operator-=(const Container& rhs)
{
    for(const auto& rh_item: rhs)
    {
        Remove(rh_item);
    }
    return *this;
}

Tuple& Tuple::operator&=(Container rhs)
{
    for(auto it=begin(); it!=end();)
    {
        auto rhit = rhs.find(*it);
        if (rhit!=rhs.end())
        {
            rhs.erase(rhit);
            ++it;
        }
        else 
            it = erase(it);
    }
    return *this;
}

Tuple& Tuple::operator|=(Container rhs)
{
    for(auto it=begin(); it!=end();++it)
    {
        auto rhit = rhs.find(*it);
        if (rhit!=rhs.end())
        {
            rhs.erase(rhit);
        }
    }
    return operator+=(rhs);
}

Set Tuple::Assumptions() const
{
    return std::accumulate(begin(), end(), Set{}, [](const Set& assumptions, const Expression& item)
    {
        return assumptions + item.Assumptions();
    });
}

Tuple::const_iterator Tuple::Find(const Expression& key) const
{
    return std::find_if(begin(), end(), [&key](const value_type& item)
    {
        if (item == key)
            return true;
        else if (const auto* association = item.GetIf<Association>())
            return association->Left() == key;
        else 
            return false;
    }); 
}

bool AllTrue(const Bag& values)
{
    for(const auto& item: values)
    {
        if (item!=True)
            return false;
    }
    return true;
}

Expression Tuple::Get(const Expression& key) const
{
    // TODO: List keys should return Lists of values, the order is not 
    // terribly useful, since indices are missing, but still it's nicer
    Bag values;
    for(const auto &item : *this)
    {
        const auto* assocation = item.GetIf<Association>();
        if (assocation)
        {
            auto maybeRight = assocation->Get(key).Simplify();
            if (maybeRight != False)
            {
                values.Add(std::move(maybeRight));
            }            
        }
        else
        {
            auto match = item.Matches(key, {});
            if (match==True)
            {
                values.Add(True);
            }
            else if (match!=False)
            {
                // adding the hypothesis now? what does [1, 2].$V do? $V=1|$V=2?
                values.Add(std::move(match));
            }
        }
    }

    if (AllTrue(values))
        return Integer(values.size());
    else
        return values;
}


unsigned Tuple::Add(Expression&& key)
{
    const_iterator back = end();
    return AddAt(back, std::move(key));
}

unsigned Tuple::AddAt(const_iterator& at, Expression&& key)
{
    const_container_iterator it(at);
    unsigned added = AddToContainer(it, std::move(key), [this](const_container_iterator& at, Expression&& item)
    {
        auto it = std::get<Tuple::const_iterator>(at);  
        at = const_container_iterator(Tuple::emplace(it, std::move(item)));
        return 1;        
    });
    at = std::get<const_iterator>(it);
    return added;

}
unsigned Tuple::Remove(const Expression& key)
{
    auto it = Find(key);
    if (it==end())
        return 0;
    erase(it);
    return 1;
}

Tuple Tuple::SimplifyItems() const 
{
    Tuple simplified; 
    simplified.reserve(size());
    std::transform(begin(), end(), std::back_inserter(simplified), [](const Expression& e)
    {
        return e.Simplify();
    });
    return simplified;   
}

Tuple Tuple::SubstituteItems(const Hypothesis& hypothesis) const
{
    Tuple substitute; 
    substitute.reserve(size());
    for(const auto& item: *this)
    {
        auto substituted = item.Substitute(hypothesis);
        substitute.Add(std::move(substituted));
    }
    return substitute;   
}

Expression Tuple::Matches(Tuple_subrange range, const Hypothesis& hypothesis) const
{
    auto range_it = range.begin();
    const_iterator it = begin();
    Hypothesis vars;
    while(it!=end() || range_it!=range.end())
    {
        Expression itemMatch;
        if (it!=end())
        {
            if (const auto* this_all = it->GetIf<All>())
            {
                assert(it+1 == end());   // for now * can only be at end
                vars.Add(this_all->Matches(Tuple_subrange(range_it, range.end()), vars, false));
                range_it=range.end();  
                ++it;
                continue;
            }
        }

        if (range_it!=range.end())
        {
            if (const auto* list_all = range_it->GetIf<All>())
            {
                assert(range_it+1 == range.end());   // for now * can only be at end
                vars.Add(list_all->Matches(Tuple_subrange(it, end()), vars, true));
                it = end(); // TODO could be partial, perhaps tuple match should return integer or (set) exactly which matched
                ++range_it;
                continue;
            }
        }

        if (it==end())
        {
            if (range_it==range.end())
            {
                break;
            }
            else 
            {
                return False; // length mismatch
            }
        }
        else if (range_it==range.end())
        {
            return False; // length mismatch
        }
        else 
        {
            auto itemMatch = it->Matches(*range_it, vars);
            if (itemMatch == False)
                return itemMatch;
            else if (itemMatch != True)
                vars.Add(std::move(itemMatch));
            ++it; ++range_it; 
        }
    }
    return vars;
}

std::size_t Tuple::Hash() const
{
    std::size_t result = 0;
    std::hash<Expression> hasher;
    for(const auto& n: *this)
    {
        result ^= hasher(n);
    }
    return result;
}

}
