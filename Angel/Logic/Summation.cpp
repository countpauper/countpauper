#include "Logic/Summation.h"
#include "Logic/Integer.h"
#include "Logic/Knowledge.h"
#include "Logic/Expression.h"
#include <numeric>

namespace Angel::Logic
{

bool Summation::operator==(const Summation& rhs) const
{
    return FlatCollection<Summation>::operator==(rhs);
}

Expression Summation::Simplify() const
{
    if (empty())
        return Integer(0);
    else if (size()==1)
        return front().Simplify();
    return SimplifyItems();
    // TODO could maybe sum them all if they are all elements
}

Match Summation::Matches(const Expression&, const Variables& vars) const
{
    // TODO: Summation matches with mathematical simplication
    // X+1 matches 3 if X is 2 
    // all unknown variables need to be found and the remaining expression(s) needs to be computed. 
    // if any are ranges, then the result might be a different range 
    return Boolean(false);
}

Summation Summation::Substitute(const Variables& substitutions) const
{
    return SubstituteItems(substitutions);
}

Expression Summation::Infer(const Knowledge& k, const Variables& substitutions) const
{
    // TODO: float and imaginary and upgrade when needed, also when overflowing
    // this can, for instance, be done by accumulating an Expression and making Objects implement operator+(Expression) etc
    long value = std::accumulate(begin(), end(), 0L,
        [&k, &substitutions](long accumulated, const Expression& item)
        {
            auto inferred = item.Infer(k, substitutions);
            auto value = inferred.Cast<Integer>();
            return accumulated += *value;
        });
    return Integer(value);
}

std::ostream& operator<<(std::ostream& os, const Summation& sum)
{
    bool first = true;
    for(const auto& obj: sum)
    {
        auto s = to_string(obj);
        if ((!first) && (!s.empty()) && (s.front()!='-'))
            os << sum.ope;
        os << s;
        first = false;
    }
    return os;
}

}