#pragma once

#include "Logic/Set.h"
#include "Logic/List.h"
#include "Logic/All.h"
#include "Internal/VariantUtils.h"
#include <variant>


namespace Angel::Logic
{

using ContainerVariant = std::variant<
    List, Set, All>;  

class Container : public ContainerVariant
{
public:
    template<typename T>
    requires is_alternative<T, Container>
    Container(const T& v) :
        ContainerVariant(v)
    {
    }
    Container& operator=(const Container& e);
    std::size_t Hash() const;
    
    // TODO: Make LogicVariant helper wrapper 
    template<class... Types>
    bool Is() const
    {
        return (std::holds_alternative<Types>(*this) || ...);
    }
        
    template<typename T>
    const T* GetIf() const 
    {
        return std::get_if<T>(this);
    }

    template<typename T> 
    const T& Get() const 
    {
        return std::get<T>(*this);
    }

/*
    template<typename T>
    const std::optional<T> TryCast() const
    {
        auto same = std::get_if<T>(this);
        if (same)
            return std::optional<T>(*same);
        else
        {
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
    }


    template<typename T>
    const T Cast() const
    {
        auto maybe = TryCast<T>();
        if (maybe)
            return *maybe;
        throw CastException(AlternativeTypeId(), typeid(T));
    }
    Expression TryCast(const std::type_info& rtt) const;
    Expression Cast(const std::type_info& rtt) const;
    */
    std::size_t size() const;

    Container Simplify() const;
    Set Assumptions() const;
    inline bool IsConstant() const { return Assumptions().empty(); }
    Container Substitute(const Hypothesis& hypothesis) const;
    Expression Matches(const Expression& e, const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& knowledge, const Hypothesis& hypothesis, Trace& trace) const;

    explicit operator bool() const;

    template<typename T> 
    requires(!std::is_same_v<Container, T>) 
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
    bool operator==(const Container& rhs) const;
 
    template<typename T> 
    bool operator<(const T& rhs) const
    {
        return std::visit(
            [this, &rhs](const auto& obj) 
            {   // TODO: could even generalize and iterate here? 
                std::hash<Expression> hasher;
                return this->Hash() < hasher(rhs);
            },*this);
    }

    bool operator<(const Container& o) const;
    inline bool operator<=(const Container& o) const { return *this==o || *this<o; }
    inline bool operator>(const Container& o) const { return !(*this<=o); }
    inline bool operator>=(const Container& o) const { return !(*this<o); }

    const std::type_info& AlternativeTypeId() const;
    std::string Summary() const;
};


std::ostream& operator<<(std::ostream& s, const Container& e);
std::string to_string(const Container& e);

}
