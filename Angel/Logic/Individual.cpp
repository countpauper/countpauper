#include "Logic/Individual.h"
#include "Logic/Expression.h"
#include <memory>

namespace Angel::Logic
{

Individual::Individual(Expression&& e) :
    content(std::make_unique<Expression>(std::move(e)))
{
}

Individual::Individual(const Individual& o) :
    content(std::make_unique<Expression>(*o.content))
{
}
Individual& Individual::operator=(const Individual& rhs)
{
    content = std::make_unique<Expression>(*rhs.content);
    return *this;
}

Individual::operator bool() const
{
    return bool(content);
}


std::size_t Individual::size() const
{
    if (content)
        return 1;
    else    
        return 0;
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
    if (!content)
        return !other.content;
    return *content == *other.content;
}

}