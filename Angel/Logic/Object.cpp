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

template<>
const std::optional<Predicate> TryCast<Predicate>(const Object& o)
{
    auto same = std::get_if<Predicate>(&o);  // TODO really cast 
    if (same)
        return std::optional<Predicate>(*same);

    return std::visit(overloaded_visit{
        [](const Id& id) { return std::optional<Predicate>(Predicate(id)); },
        [](const auto&) { return std::optional<Predicate>(); }
    }, o);
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
