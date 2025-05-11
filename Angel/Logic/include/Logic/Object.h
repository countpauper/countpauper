#pragma once

#include "Logic/Element.h"
#include "Logic/Predicate.h"
#include "Logic/Set.h"
#include <variant>
#include <vector>
#include <map>
#include <type_traits>


namespace Angel::Logic
{
    class Node;
}

namespace std
{
	template <>
	struct hash<Angel::Logic::Node>
	{
		size_t operator()(const Angel::Logic::Node& n) const;
	};
}

namespace Angel::Logic
{
/*

class Knowledge;
class Expression;

// An object is a variadic instance of any type of expr
class Object final
{
public:
    Object() = default;
    explicit Object(std::unique_ptr<Expression>&& v) :
        expr(std::move(v))
    {
    }

    Object(const Object& other);
	Object& operator=(const Object& other);
	Object(Object&& other);
	Object& operator=(Object&& other);

	operator bool() const;
	bool null() const;
    bool operator==(const Object& other) const;
    bool operator!=(const Object& other) const { return !operator==(other); }
	operator std::string() const;

    const Expression& operator*() const;
    const Expression* operator->() const;
    Object Infer(const Knowledge& knowledge, const std::map<std::string, Object>& substitutions={}) const;
	template<class C>
    C* As() const { return dynamic_cast<C*>(expr.get()); }
    template<class C>
    Object Cast(const Knowledge& knowledge) const { return Cast(typeid(C), knowledge); } 
    std::size_t Hash() const;
private:
    friend class Expression;
    Object Cast(const std::type_info& t, const Knowledge& knowledge) const;
	std::unique_ptr<Expression> expr;
};

template<class T, typename... Args>
Object Create(Args&&... args)
{
	return Object(std::make_unique<T>(std::forward<Args>(args)...));
}


*/


template<typename T>
using Collection  = std::vector<T>;


struct Node;

class List : public Collection<Node>
{
public:
	explicit List(std::initializer_list<Node> setItems);
    template<typename T> 
    explicit List(std::initializer_list<T> items)
    {
        for(auto&& item: items)
        {
            emplace_back(item);
        }
    }
    bool operator==(const List& rhs) const;
    Node operator[](const int idx);
    std::size_t Hash() const;
};

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
