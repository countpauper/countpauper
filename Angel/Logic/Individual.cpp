#include "Logic/Internal/Individual.h"
#include "Logic/Expression.h"
#include <memory>

namespace Angel::Logic
{

Individual::Individual(const Expression& e) :
    content(bool(e)?std::make_unique<Expression>(e):nullptr)
{
}


Individual::Individual(Expression&& e) :
    content(bool(e)?std::make_unique<Expression>(std::move(e)):nullptr)
{
}

Individual::Individual(const Individual& o) :
    content(o.empty()?nullptr:std::make_unique<Expression>(*o.content))
{
}

Individual::Individual(Individual&& o) :
    content(std::move(o.content))
{
}

Individual& Individual::operator=(const Individual& rhs)
{
    content = std::make_unique<Expression>(*rhs.content);
    return *this;
}

Individual& Individual::operator=(Individual&& rhs)
{
    content.reset();
    std::swap(content, rhs.content);
    return *this;
}


Individual::operator bool() const
{
    return !empty();
}


std::size_t Individual::size() const
{
    if (empty())
        return 0;
    else    
        return 1;
}

bool Individual::empty() const
{
    return !bool(content);
}

Expression Individual::RemoveLeft()
{
    if (empty())
        return Expression();

    Expression operand = std::move(*content);
    content.reset();
    return operand; 
}

void Individual::AddLeft(Expression&& operand)
{
    if (!empty())
        throw std::runtime_error("Individual buffer overflow");
    content = std::make_unique<Expression>(std::move(operand));
}

Expression Individual::RemoveRight()
{
    return RemoveLeft();
}

void Individual::AddRight(Expression&& operand)
{
    AddLeft(std::move(operand));
}

Set Individual::Assumptions() const
{
    return content->Assumptions();
}

const Expression& Individual::operator*() const
{
    return *content;
}

const Expression* Individual::operator->() const
{
    return content.get();
}

std::size_t Individual::Hash() const
{
    if (!content) 
        return 0;
    std::hash<Expression> hash;
    return hash(*content);
}

bool Individual::operator==(const Individual& other) const
{
    if (empty())
        return other.empty();
    if (other.empty())
        return false;
    return *content == *other.content;
}

}