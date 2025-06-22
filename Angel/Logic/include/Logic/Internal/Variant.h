#pragma once

#include <variant>
#include "Logic/CastException.h"

namespace Angel::Logic
{

template<typename... Alternatives> 
class Variant : public std::variant<Alternatives...>
{
public:
    using VariantT = std::variant<Alternatives...>;
    Variant() :
        VariantT(std::variant_alternative_t<0, VariantT>())
    {
    }
    template<typename T>
    requires is_alternative<T, VariantT>
    Variant(const T& v) :
        VariantT(v)
    {
    }


    std::size_t Hash() const
    {
        return std::visit(
            [this](const auto& obj)
            {
                return obj.Hash();
            }, *this);       
    }


    template<class T>
    requires is_alternative<T, VariantT>
    bool Is() const
    {
        return std::holds_alternative<T>(*this);
    }    

    template<class T>
    requires (!is_alternative<T, VariantT>)
    bool Is() const
    {
        return false;
    }  

    template<typename T>
    requires is_alternative<T, VariantT>
    const T* GetIf() const 
    {
        return std::get_if<T>(this);
    }

    template<typename T>
    requires is_alternative<T, VariantT>
    T* GetIf() 
    {
        return std::get_if<T>(this);
    }
    template<typename T> T* GetIf() const 
    { 
        return nullptr; 
    };

    template<typename T> 
    requires is_alternative<T, VariantT>
    const T& Get() const 
    {
        return std::get<T>(*this);
    }

    template<typename T> 
    requires is_alternative<T, VariantT>
    T& Get() 
    {
        return std::get<T>(*this);
    }

    template<typename T>
    const std::optional<T> TryCast() const
    {
        auto same = GetIf<T>();
        if (same)
            return std::optional<T>(*same);
        return std::visit(overloaded_visit{
            // Automatically cast any alternative that has a constructor that 
            // takes another alternative const& as its sole argument
            [this](const auto& castee) 
            requires std::is_constructible_v<T, decltype(castee)>
            {
                return std::optional<T>(castee);
            },
            [](const auto&) 
            {
                return std::optional<T>();
            }
        }, *this);
    }

    template<typename T>
    const T Cast() const
    {
        auto maybe = TryCast<T>();
        if (maybe)
            return *maybe;
        throw CastException(AlternativeTypeId(), typeid(T));
    }


    template<typename T> 
    requires is_alternative<T, VariantT>
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

    // Expression TryCast(const std::type_info& rtt) const;
    // Expression Cast(const std::type_info& rtt) const;
    const std::type_info& AlternativeTypeId() const 
    {
        return *std::visit([](const auto& obj)
        {
            return &typeid(decltype(obj)); 
        }, *this);        
    }
};

}
