#include "Logic/Internal/container_iterator.h"
#include "Logic/Expression.h"
#include "Logic/Internal/VariantUtils.h"


namespace Angel::Logic
{

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