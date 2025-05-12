#include "Logic/Object.h"
#include "Logic/Boolean.h"
#include "Logic/Integer.h"
#include "Logic/Id.h"
#include "Logic/Expression.h"
#include "Logic/Knowledge.h"
#include <sstream>
#include <cassert>

namespace Angel::Logic
{

Object::Object(const Object& o) :
    ObjectVariant(o)
{
}

Element Object::Compute(const class Knowledge& knowledge, const Variables& vars) const
{
    return std::visit(overloaded_visit{
        [&knowledge, &vars](const Id& id) { 
            return id ? Predicate(id).Compute(knowledge, vars) : Boolean(false);
        },   // TODO id compute implement like this
        [](const List&) -> Element {
            assert(false && "TODO: can't compute a container to an element. Take size? ");
            return Boolean(false);
        }, 
        [](const Set&) -> Element {
            assert(false && "TODO: can't compute a container to an element. Take size? ");
            return Boolean(false);
        }, 
        []<IsElement T>(const T& element) -> Element {
            return element; 
        },
        [&knowledge, &vars](const auto& obj) -> Element {
            return obj.Compute(knowledge, vars);
        }
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
