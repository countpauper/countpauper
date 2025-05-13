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

Object Summation::Compute(const Knowledge& k, const Variables& substitutions) const
{
    // TODO: float and imaginary and upgrade when needed, also when overflowing
    // this can, for instance, be done by accumulating an Object and making Objects implement operator+(Object) etc
    long value = std::accumulate(begin(), end(), 0L,
        [&k, &substitutions](long accumulated, const Object& item)
        {
            auto object = k.Compute(item);
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