#include "Logic/Object.h"
#include "Logic/Boolean.h"
#include "Logic/Integer.h"
#include "Logic/Id.h"
#include "Logic/Expression.h"
#include <sstream>

namespace Angel::Logic
{

std::ostream& operator<<(std::ostream& s, const Object& o)
{
    std::visit([&s](const auto& obj)
    {
        s << obj;
    }, o);
    return s;
}
}


namespace std
{

size_t hash<Angel::Logic::Node>::operator()(const Angel::Logic::Node& n) const
{
    std::hash<Angel::Logic::Object> hasher;
    return hasher(n.value);
}

size_t hash<Angel::Logic::Object>::operator()(const Angel::Logic::Object& o) const
{
    return std::visit([](const auto& obj) { return obj.Hash(); }, o);
}


}
