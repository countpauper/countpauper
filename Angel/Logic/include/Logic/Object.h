#pragma once

#include "Logic/Element.h"
#include "Logic/Predicate.h"
#include "Logic/Set.h"
#include "Logic/List.h"
#include "Logic/CastException.h"
#include "Logic/VariantUtils.h"
#include <variant>
#include <vector>
#include <map>
#include <type_traits>

namespace Angel::Logic
{

using VariantObject = std::variant<Boolean,  Integer, Id, Variable, Predicate, List, Set>; 

class Object : public VariantObject
{
public:
    template<typename T>
    requires is_alternative<T, VariantObject>
    Object(const T& v) :
        VariantObject(v)
    {
    }
    Object(const Object& n);


    template<typename T>
    const std::optional<T> TryCast() const
    {
        auto same = std::get_if<T>(this);  // TODO really cast 
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

template<>
const std::optional<Predicate> Object::TryCast<Predicate>() const;

std::ostream& operator<<(std::ostream& s, const Object& o);
}

namespace std
{
	template <>
	struct hash<Angel::Logic::Object>
	{
		size_t operator()(const Angel::Logic::Object& n) const;
	};
}