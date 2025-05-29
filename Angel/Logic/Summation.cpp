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
    Summation simple = SimplifyItems();
    long sum = 0;
    auto it = simple.begin();
    while(it!=simple.end())
    {
        auto integer = it->TryCast<Integer>();        
        if (integer)
        {
            sum += **integer;
            it = simple.erase(it);
        }
        else 
            ++it;
    }
    if (sum)
        simple.push_back(Integer(sum));
    if (simple.empty())
        return Integer(0);
    else if (simple.size()==1)
        return simple.front();
    else
        return std::move(simple);
}

Expression Summation::Matches(const Expression&, const Substitutions& substitutions) const
{
    // TODO: Summation matches with mathematical simplication
    // X+1 matches 3 if X is 2 
    // all unknown variables need to be found and the remaining expression(s) needs to be computed. 
    // if any are ranges, then the result might be a different range 
    return Boolean(false);
}

Summation Summation::Substitute(const Substitutions& substitutions) const
{
    return SubstituteItems(substitutions);
}

Expression Summation::Infer(const Knowledge& k, const Substitutions& substitutions) const
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