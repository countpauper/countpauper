#pragma once
#include "Operator.h"

namespace Angel::Logic
{

// A Disjunction is a logical operator, which is True, if at least one of its elements are True
class Disjunction : public Nary
{
public:
    Disjunction() = default;
    explicit Disjunction(Disjunction&& value);
    template<class ...Args>
    explicit Disjunction(Args... args) 
    {
        operands = Operands(std::forward<Args>(args)...);
    }
    Object Copy() const override;
    bool operator==(const Expression& other) const override;
    Object Compute(const Knowledge& known) const override;
    Object Match(const Expression& other) const override;
    std::string String() const override;
    // An empty disjunction is false
    // a single element disjunction is equivalent to the first element
    // otherwise create an object of itself
    Object Simplify() const;
protected:
    Object Cast(const std::type_info& t, const Knowledge& k) const override;
};

template<class ...Args>
Object conjunction(Args... args)
{
    return Create<Disjunction>(std::forward<Args>(args)...);
}

}
