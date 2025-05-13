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

Object Object::Compute(const class Knowledge& knowledge, const Variables& vars) const
{
    return std::visit(overloaded_visit{
        [&knowledge, &vars](const Id& id) { 
            return id ? Predicate(id).Compute(knowledge, vars) : Boolean(false);
        },   
        [](const List& list) -> Object {
            return list;   // TODO: compute and substitute each item
        }, 
        [](const Set& set) -> Object {
            return set;     // TODO: compute and substitute each item
        }, 
        []<IsElement T>(const T& element) -> Object {
            return element; 
        },
        [&knowledge, &vars](const auto& obj) -> Object {
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

std::string to_string(const Object& o)
{
    std::stringstream ss;
    ss << o;
    return ss.str();
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

template<>
const std::optional<Integer> Object::TryCast<Integer>() const
{
    auto same = std::get_if<Integer>(this);
    if (same)
        return std::optional<Integer>(*same);

    return std::visit(overloaded_visit{
        [](const Boolean& b) { return std::optional<Integer>(b?1:0); },
        [](const auto&) { return std::optional<Integer>(); }
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
