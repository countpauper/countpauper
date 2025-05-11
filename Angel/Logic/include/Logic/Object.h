#pragma once

#include "Logic/Node.h"
#include "Logic/Element.h"
#include "Logic/Predicate.h"
#include "Logic/Set.h"
#include "Logic/List.h"
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
