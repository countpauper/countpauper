#include "Logic/Container.h"
#include "Logic/Expression.h"
#include "Logic/Internal/container_iterator.h"
#include <sstream>
#include <cassert>

namespace Angel::Logic
{

Container& Container::operator=(const Container& e)
{
    ContainerVariant::operator=(e);
    return *this;
}

std::size_t Container::size() const
{
    return std::visit(
        [this](const auto& obj)
        {
            return obj.size();
        }, *this);   
}

Container Container::Simplify() const
{
    return std::visit(overloaded_visit{
        [this](const auto& obj) -> Container
        {
            return Container(obj.Simplify());
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
            return Container(obj.Substitute(hypothesis));
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

const_container_iterator Container::begin() const
{
    return std::visit(
         [](const auto& obj) 
         {
            return const_container_iterator{obj.begin()};
         }, *this);
}

const_container_iterator Container::end() const
{
    return std::visit(
         [](const auto& obj) 
         {
            return const_container_iterator{obj.end()};
         }, *this);
        }
bool Container::operator<(const Container& o) const
{
    return Hash() < o.Hash();
}

std::string Container::Summary() const
{
    return std::visit(overloaded_visit{
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

// TODO: move to file and header to internal 
Expression const_container_iterator::operator*() const
{
    return std::visit(overloaded_visit{
        [](const std::monostate&)
        {
            return Expression();
        },
        [](const auto& it)
        {
            return *it;
        }}, *this);
}

const_container_iterator& const_container_iterator::operator++()
{
    std::visit(overloaded_visit{
        [](std::monostate)
        {
            throw std::runtime_error("Can't iterate over a a non-iterator");
        },
        [](auto& it)
        {
            ++it;
        }}, *this);
    return *this;
}

const_container_iterator const_container_iterator::operator++(int) 
{ 
    const_container_iterator tmp(*this);
    ++*this;
    return tmp; 
}

const_container_iterator::operator bool() const
{
    return !std::holds_alternative<std::monostate>(*this);
}


}