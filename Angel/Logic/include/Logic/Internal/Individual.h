#pragma once 
#include <memory>

namespace Angel::Logic
{
class Expression;
class Set;

class Individual
{
public: 
    explicit Individual(const Expression& e);
    explicit Individual(Expression&& e);
    Individual(const Individual& o);
    Individual(Individual&& o);
    
    Individual& operator=(const Individual& rhs);
    Individual& operator=(Individual&& rhs);
    explicit operator bool() const;
    std::size_t size() const;
    bool empty() const;
    Set Assumptions() const;
    const Expression& operator*() const;
    const Expression* operator->() const;
    std::size_t Hash() const;
protected:
    bool operator==(const Individual& other) const;
    std::unique_ptr<Expression> content;
};

}