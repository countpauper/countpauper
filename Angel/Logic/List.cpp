#include "Logic/List.h"
#include "Logic/Object.h"
#include <typeinfo>
#include <iostream>

namespace Angel::Logic
{
    
List::List(std::initializer_list<Node> listItems) :
    std::vector<Node>(listItems)
{
}

bool List::operator==(const List& rhs) const
{
    if (size()!=rhs.size())
        return false;
    return std::equal(begin(), end(), rhs.begin());
}

List::operator bool() const
{
    return !empty();
}


std::size_t List::Hash() const
{
    std::size_t result = typeid(decltype(*this)).hash_code();
    std::hash<Node> hasher;
    for(const auto& n: *this)
    {
        result ^= hasher(n);
    }
    return result;
}


std::ostream& operator<<(std::ostream& os, const List& list)
{
    os << "[";
    bool first = true;
    for(const auto& node: list)
    {
        if (!first)
            os << ",";
        os << node.value;
        first = false;
    }

    os << "]";
    return os;
}
}

