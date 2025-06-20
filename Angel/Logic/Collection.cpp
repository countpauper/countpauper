#include "Logic/Internal/Collection.h"
#include "Logic/Expression.h"

#include <typeinfo>
#include <iostream>
#include <cassert>
#include <numeric>

namespace Angel::Logic
{

Collection::Collection(std::initializer_list<Expression> items) :
    std::vector<Expression>(items)
{
}

Collection::Collection(std::vector<Expression>&& items) :
    std::vector<Expression>(std::move(items))
{
}

Collection::Collection(Collection_subrange items)
{
    reserve(std::ranges::size(items));
    for (auto&& e : items) {
        emplace_back(e);
    }    
}

Collection::~Collection()
{
}

bool Collection::operator==(const Collection& rhs) const
{
    if (size()!=rhs.size())
        return false;
    return std::equal(begin(), end(), rhs.begin());
}

Collection& Collection::operator+=(const Collection& rhs)
{
    insert(end(), rhs.begin(), rhs.end());
    return *this;
}

Collection& Collection::operator-=(const Collection& rhs)
{
    for(const auto& rh_item: rhs)
    {
        Remove(rh_item);
    }
    return *this;
}

Collection& Collection::operator&=(Collection rhs)
{
    for(auto it=begin(); it!=end();)
    {
        auto rhit = rhs.Find(*it);
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

Collection& Collection::operator|=(Collection rhs)
{
    for(auto it=begin(); it!=end();++it)
    {
        auto rhit = rhs.Find(*it);
        if (rhit!=rhs.end())
        {
            rhs.erase(rhit);
        }
    }
    return operator+=(rhs);
}

Set Collection::Assumptions() const
{
    return std::accumulate(begin(), end(), Set{}, [](const Set& assumptions, const Expression& item)
    {
        return assumptions + item.Assumptions();
    });
}

Collection::const_iterator Collection::Find(const Expression& key) const
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

Expression Collection::Get(const Expression& key) const
{
    // TODO: Lists should return Lists of values, the order is not 
    // terribly useful, since indices are missing, but still it's nicer
    Bag values;
    bool return_list = false;
    for(const auto &item : *this)
    {
        const auto* assocation = item.GetIf<Association>();
        if (assocation)
        {
            auto maybeRight = assocation->Get(key);
            if (maybeRight != Boolean(false))
            {
                values.emplace_back(assocation->Right());
                return_list = true;
            }            
        }
        else
        {
            auto match = item.Matches(key, {});
            if (match==Boolean(true))
            {
                values.emplace_back(Boolean(true));
            }
            else if (match!=Boolean(false))
            {
                // adding the hypothesis now? what does [1, 2].$V do? $V=1|$V=2?
                values.emplace_back(match);
                return_list = true; 
            }
        }
    }
    if (return_list)
        return values;
    else
        return Integer(values.size());
}


unsigned Collection::Add(const Expression& key)
{
    push_back(key);
    return 1;
}
unsigned Collection::Remove(const Expression& key)
{
    auto it = Find(key);
    if (it==end())
        return 0;
    erase(it);
    return 1;
}

Collection Collection::SimplifyItems() const 
{
    Collection simplified; 
    simplified.reserve(size());
    std::transform(begin(), end(), std::back_inserter(simplified), [](const Expression& e)
    {
        return e.Simplify();
    });
    return simplified;   
}

Collection Collection::SubstituteItems(const Hypothesis& hypothesis) const
{
    Collection substitute; 
    substitute.reserve(size());
    for(const auto& item: *this)
    {
        /*
        auto substituted = item.Substitute(hypothesis);
        if (const All* all = item.GetIf<All>())
        {
            for(auto subItem: all)
            {
                substitute.emplace_back(subItem);
            }
        }
        else 
        {
            substitute.emplace_back(substituted);
        }
        */
        if (const Tuple* tuple = item.GetIf<Tuple>())
        {
            auto tupstitution = tuple->Substitute(hypothesis);
            if (const List* insert = tupstitution.GetIf<List>()) // TODO could be other containers
            {
                substitute.insert(substitute.end(), insert->begin(), insert->end());
            }
            else
            {   // insert the unsubstituted tuple 
                substitute.emplace_back(std::move(tupstitution));
            }
            continue;
        }
        substitute.emplace_back(item.Substitute(hypothesis));
    }
    return substitute;   
}

Expression Collection::Matches(Collection_subrange range, const Hypothesis& hypothesis) const
{
    auto range_it = range.begin();
    const_iterator it = begin();
    Hypothesis vars;
    while(it!=end() || range_it!=range.end())
    {
        Expression itemMatch;
        if (it!=end())
        {
            if (const auto* this_tuple = it->GetIf<Tuple>())
            {
                assert(it+1 == end());   // for now * can only be at end
                vars.emplace_back(this_tuple->Matches(Collection_subrange(range_it, range.end()), vars, false));
                range_it=range.end();  
                ++it;
                continue;
            }
        }

        if (range_it!=range.end())
        {
            if (const auto* list_tuple = range_it->GetIf<Tuple>())
            {
                assert(range_it+1 == range.end());   // for now * can only be at end
                vars.emplace_back(list_tuple->Matches(Collection_subrange(it, end()), vars, true));
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
                return Boolean(false); // length mismatch
            }
        }
        else if (range_it==range.end())
        {
            return Boolean(false); // length mismatch
        }
        else 
        {
            auto itemMatch = it->Matches(*range_it, vars);
            if (itemMatch == Boolean(false))
                return itemMatch;
            else if (itemMatch != Boolean(true))
                vars.emplace_back(std::move(itemMatch));
            ++it; ++range_it; 
        }
    }
    return vars;
}

std::size_t Collection::Hash() const
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
