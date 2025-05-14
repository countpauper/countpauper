#pragma once

#include "Logic/Element.h"
#include "Logic/Predicate.h"
#include "Logic/Set.h"
#include "Logic/List.h"
#include "Logic/CastException.h"
#include "Logic/Conjunction.h"
#include "Logic/Disjunction.h"
#include "Logic/Summation.h"
#include "Logic/VariantUtils.h"
#include <variant>
#include <vector>
#include <map>
#include <type_traits>

namespace Angel::Logic
{

using ObjectVariant = std::variant<Boolean,  Integer, Id, Variable, Predicate, 
    List, Set, 
    Conjunction, Disjunction, Summation>; 

class Object : public ObjectVariant
{
public:
    template<typename T>
    requires is_alternative<T, ObjectVariant>
    Object(const T& v) :
        ObjectVariant(v)
    {
    }
    Object(const Object& o);

    template<typename T>
    const std::optional<T> TryCast() const
    {
        auto same = std::get_if<T>(this);
        if (same)
            return std::optional<T>(*same);
        else
            return std::optional<T>();
    }

    const std::type_info& AlternativeTypeInfo() const 
    {
        return *std::visit([](const auto& obj)
        {
            return &typeid(decltype(obj)); 
        }, *this);        
    }

    template<typename T>
    const T Cast() const
    {
        auto maybe = TryCast<T>();
        if (maybe)
            return *maybe;
        throw CastException(AlternativeTypeInfo(), typeid(T));

    }
    Match Matches(const Object& o, const Variables& substitutions) const;
    Object Compute(const class Knowledge& knowledge, const Variables& substitutions) const;

    template<typename T> 
    requires(!std::is_same_v<Object, T>) 
    bool operator==(const T& rhs) const
    {
        return std::visit(overloaded_visit{
            [&rhs](const T& lv)
            {
                return lv == rhs;
            }, 
            [](const auto&) { return false; }   
            },*this);
    }

    bool operator<(const Object&o) const;
};

template<> const std::optional<Predicate> Object::TryCast<>() const;
template<> const std::optional<Integer> Object::TryCast<>() const;

std::ostream& operator<<(std::ostream& s, const Object& o);

std::string to_string(const Object& o);
}

namespace std
{
	template <>
	struct hash<Angel::Logic::Object>
	{
		size_t operator()(const Angel::Logic::Object& n) const;
	};
}