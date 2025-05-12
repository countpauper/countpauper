#pragma once

#include "Logic/Node.h"
#include "Logic/Element.h"
#include "Logic/Predicate.h"
#include "Logic/Set.h"
#include "Logic/List.h"
#include "Logic/CastException.h"
#include <variant>
#include <vector>
#include <map>
#include <type_traits>


namespace Angel::Logic
{

using Object = std::variant<Boolean,  Integer, Id, Variable, Predicate, List, Set>; 

template<class... Ts> struct overloaded_visit : Ts... { using Ts::operator()...; };

template<typename T> 
requires(!std::is_same_v<Object, T>) 
bool operator==(const Object& left, const T& right)
{
    return std::visit(overloaded_visit{
        [&right](const T& lv)
        {
            return lv == right;
        }, 
        [](const auto&) { return false; }   
        },left);
}

template<typename T>
const std::optional<T> TryCast(const Object& o)
{
    auto same = std::get_if<T>(&o);  // TODO really cast 
    if (same)
        return std::optional<T>(*same);
    else
        return std::optional<T>();
}

template<>
const std::optional<Predicate> TryCast<Predicate>(const Object& o);

template<typename T>
const T Cast(const Object& o)
{
    auto maybe = TryCast<T>(o);
    if (maybe)
        return *maybe;
    else 
    {
        std::visit([](const auto& obj)
        {
            throw CastException<decltype(obj)>(typeid(T)); return T();
        }, o);
    }
}

struct Node {
    Object value;
    bool operator<(const Node&o) const;
    bool operator==(const Node& o) const;

};

std::ostream& operator<<(std::ostream& s, const Object& o);
}

namespace std
{
    template <>
	struct hash<Angel::Logic::Object>
	{
		size_t operator()(const Angel::Logic::Object& o) const;
	};
}
