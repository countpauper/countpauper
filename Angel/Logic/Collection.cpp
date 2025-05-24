#include "Logic/Collection.h"
#include "Logic/Expression.h"

#include <typeinfo>
#include <iostream>

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

Collection::~Collection()
{
}

bool Collection::operator==(const Collection& rhs) const
{
    if (size()!=rhs.size())
        return false;
    return std::equal(begin(), end(), rhs.begin());
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
        else if (item == key)
            values.emplace_back(Boolean(true));
    }
    if (return_list)
        return values;
    else
        return Integer(values.size());
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
