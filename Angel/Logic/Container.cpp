#include "Logic/Container.h"
#include "Logic/Expression.h"
#include <sstream>
#include <cassert>

namespace Angel::Logic
{

Container& Container::operator=(const Container& e)
{
    ContainerVariant::operator=(e);
    return *this;
}

std::size_t Container::Hash() const
{
    return std::visit(
        [this](const auto& obj)
        {
            return obj.Hash();
        }, *this);       
}


std::size_t Container::size() const
{
    return std::visit(
        [this](const auto& obj)
        {
            return obj.size();
        }, *this);   
}

/*
template <std::size_t idx>
constexpr Container CastIterate(const Container&e, const std::type_info& rtt) 
{
    if constexpr (idx < std::variant_size_v<ElementVariant>)
    {
        if (typeid(std::variant_alternative_t<idx, ElementVariant>) == rtt)
        {
            return e.Cast<std::variant_alternative_t<idx, ElementVariant>>();
        }
        else 
        {
            return CastIterate<idx+1>(e, rtt);
        }
    }
    return Container();
}

Container Container::TryCast(const std::type_info& rtt) const
{
    return CastIterate<0>(*this, rtt);
}

Container Container::Cast(const std::type_info& rtt) const
{
    Container result = CastIterate<0>(*this, rtt);
    if (result) 
        return result;
    else 
        throw CastException(AlternativeTypeId(), rtt); 
}
*/

Container Container::Simplify() const
{
    return std::visit(overloaded_visit{
        [this](const auto& obj) -> Container
        {
            return obj.Simplify();
        }}, *this);      
}

Set Container::Assumptions() const
{
    return std::visit(
        [this](const auto& obj)
        {
            return obj.Assumptions();
        }, *this);

}


Container Container::Substitute(const Hypothesis& hypothesis) const
{
    return std::visit(
        [&hypothesis](const auto& obj) -> Container
        {
            return obj.Substitute(hypothesis);
        },   *this);      
}


Expression Container::Matches(const Expression& e, const Hypothesis& hypothesis) const 
{
    return std::visit(
        [&e, &hypothesis](const auto& obj) 
        {
            return obj.Matches(e, hypothesis);
        },   *this);    
}

Expression Container::Infer(const class Knowledge& knowledge, const Hypothesis& hypothesis, Trace& trace) const
{
    return std::visit(
        [&knowledge, &hypothesis, &trace](const auto& obj) -> Expression {
            return obj.Infer(knowledge, hypothesis, trace);
        }, *this);
}

Container::operator bool() const
{
    return size()!=0;
}

bool Container::operator==(const Container& rhs) const
{
    return std::visit(
        [this](const auto& rho)
        {
            return operator==(rho);
        }, rhs);
}

bool Container::operator<(const Container& o) const
{
    return Hash() < o.Hash();
}

const std::type_info& Container::AlternativeTypeId() const 
{
    return *std::visit([](const auto& obj)
    {
        return &typeid(decltype(obj)); 
    }, *this);        
}

std::string Container::Summary() const
{
    return std::visit(Logic::overloaded_visit{
         [this](const All& all) -> std::string 
         {
            if (all)
            {
                return std::format("All of {}", all->Summary());
            }
            else 
            {
                return "All";
            }
        },
        [](const auto& container) -> std::string 
        {
            return std::format("{} {} container with {} items", 
                container.unique ? "unique" : "non-unique",
                container.ordered ? "ordered" : "unordered", 
                container.size()); 
        }
    }, *this);
}

std::ostream& operator<<(std::ostream& s, const Container& c)
{
    std::visit(
        [&s](const auto& obj)
        {
            s << obj;
        }, c);
    return s;
}

std::string to_string(const Container& c)
{
    std::stringstream ss;
    ss << c;
    return ss.str();
}

}
