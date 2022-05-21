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
    explicit Disjunction(Set&& operands);
    template<class ...Args>
    explicit Disjunction(Args... args) :
        Disjunction(Set(std::forward<Args>(args)...))
    {
    }
    Object Copy() const override;
    bool operator==(const Expression& other) const;
    Object Compute(const Knowledge& known) const override;
    Object Match(const Expression& other) const override;
protected:
    Object Cast(const std::type_info& t, const Knowledge& k) const override;
};

template<class ...Args>
Object conjunction(Args... args)
{
    return Create<Disjunction>(std::forward<Args>(args)...);
}

}
