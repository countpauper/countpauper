#pragma once
#include "Operator.h"

namespace Angel::Logic
{
    class Sequence;
    class Set;

// A Conjunction is a logical operator, which is true, only if all its elements are True
class Conjunction : public Nary
{
public:
    Conjunction() = default;
    explicit Conjunction(Conjunction&& value);
    explicit Conjunction(const Operands& value);
    template<class ...Args>
    explicit Conjunction(Logic::Object first, Args... args) :
        Conjunction(std::forward<Args>(args)...)
    {
        operands.insert(operands.begin(), first);
    }
    Object Copy() const override;
    bool operator==(const Expression& other) const override;
    Object Compute(const Knowledge& known) const override;
    Object Match(const Expression& other) const override;
    std::string String() const override;
protected:
    Object Cast(const std::type_info& t, const Knowledge& k) const override;
};

template<class ...Args>
Object conjunction(Args... args)
{
    return Create<Conjunction>(std::forward<Args>(args)...);
}


}