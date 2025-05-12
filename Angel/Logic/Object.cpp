#include "Logic/Object.h"
#include "Logic/Boolean.h"
#include "Logic/Integer.h"
#include "Logic/Id.h"
#include "Logic/Expression.h"
#include "Logic/Knowledge.h"
#include <sstream>

namespace Angel::Logic
{

Object::Object(const Object& n) :
    VariantObject(n)
{
}

Object Object::Compute(const class Knowledge& knowledge, const Variables& substitutions) const
{
    return std::visit(overloaded_visit{
        [&knowledge](const Predicate& predicate) { return knowledge.Query(predicate); },
        [&knowledge](const Id& id) { return bool(id)? knowledge.Query(Predicate(id)) : Boolean(false); },
        [](const auto& obj) { return Object(obj); }
    }, *this);
}

bool Object::operator<(const Object&o) const
{
    std::hash<Object> hasher; 
    return hasher(*this) < hasher(o);
}

std::ostream& operator<<(std::ostream& s, const Object& o)
{
    std::visit([&s](const auto& obj)
    {
        s << obj;
    }, o);
    return s;
}

template<>
const std::optional<Predicate> Object::TryCast<Predicate>() const
{
    auto same = std::get_if<Predicate>(this);  // TODO really cast 
    if (same)
        return std::optional<Predicate>(*same);

    return std::visit(overloaded_visit{
        [](const Id& id) { return std::optional<Predicate>(Predicate(id)); },
        [](const auto&) { return std::optional<Predicate>(); }
    }, *this);
}
}


namespace std
{

size_t hash<Angel::Logic::Object>::operator()(const Angel::Logic::Object& o) const
{
    return std::visit([](const auto& obj) { return obj.Hash(); }, o);
}


}
