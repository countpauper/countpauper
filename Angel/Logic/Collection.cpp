#include "Logic/Collection.h"
#include "Logic/Object.h"

#include <typeinfo>
#include <iostream>

namespace Angel::Logic
{

    
Collection::Collection(std::initializer_list<Object> items) :
    std::vector<Object>(items)
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

std::size_t Collection::Hash() const
{
    std::size_t result = 0;
    std::hash<Object> hasher;
    for(const auto& n: *this)
    {
        result ^= hasher(n);
    }
    return result;
}

}
