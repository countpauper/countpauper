#pragma once 
#include <memory>

namespace Angel::Logic
{
class Expression;

class Individual
{
public: 
    Individual(const Individual& o);
    Individual(Individual&& o);
    Individual& operator=(const Individual& rhs);
    Individual& operator=(Individual&& rhs);
    explicit Individual(Expression&& e);
    explicit operator bool() const;
    std::size_t size() const;
    const Expression& operator*() const;
    const Expression* operator->() const;
    std::size_t Hash() const;
protected:
    bool operator==(const Individual& other) const;
    std::unique_ptr<Expression> content;
};

}