#include "Logic/Summation.h"
#include "Logic/Integer.h"
#include "Logic/Knowledge.h"
#include <numeric>
namespace Angel::Logic
{

bool Summation::operator==(const Summation& rhs) const
{
    return Collection::operator==(rhs);
}

Match Summation::Matches(const Object& object, const Variables& vars) const
{
    // TODO: Summation matches with mathematical simplication
    // X+1 matches 3 if X is 2 
    // all unknown variables need to be found and the remaining expression(s) needs to be computed. 
    // if any are ranges, then the result might be a different range 
    return NoMatch;
}

Object Summation::Infer(const Knowledge& k, const Variables& substitutions) const
{
    // TODO: float and imaginary and upgrade when needed, also when overflowing
    // this can, for instance, be done by accumulating an Object and making Objects implement operator+(Object) etc
    long value = std::accumulate(begin(), end(), 0L,
        [&k, &substitutions](long accumulated, const Object& item)
        {
            auto object = k.Infer(item);
            auto value = object.Cast<Integer>();
            return accumulated += *value;
        });
    return Integer(value);
}

std::size_t Summation::Hash() const
{
    return typeid(decltype(*this)).hash_code() ^ Collection::Hash();
}

std::ostream& operator<<(std::ostream& os, const Summation& list)
{
    bool first = true;
    for(const auto& obj: list)
    {
        auto s = to_string(obj);
        if ((!first) && (!s.empty()) && (s.front()!='-'))
            os << "+";
        os << s;
        first = false;
    }
    return os;
}

}