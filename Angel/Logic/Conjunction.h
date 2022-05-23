#pragma once
#include "Operator.h"

namespace Angel::Logic
{

// A Conjunction is a logical operator, which is true, only if all its elements are True
class Conjunction : public Nary
{
public:
    Conjunction() = default;
    explicit Conjunction(Conjunction&& value);
    explicit Conjunction(Set&& operands);
    template<class ...Args>
    explicit Conjunction(Args... args) :
        Conjunction(Set(std::forward<Args>(args)...))
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
    return Create<Conjunction>(std::forward<Args>(args)...);
}
Object conjunction(Sequence&& seq);
Object conjunction(Set&& set);

}