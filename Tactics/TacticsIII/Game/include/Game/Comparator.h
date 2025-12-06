#pragma once
#include <string>

namespace Game
{

    //  TODO: Merge with operator and Computation 
class Comparator
{
public:
    enum Id
    {
        not_equal = 0,
        equal = 1,
        less = 2,
        less_equal = 3,
        greater = 4,
        greater_equal = 5,
    };
    Comparator(Id id);
    Comparator Opposite() const;
    operator std::string() const;
    bool operator==(Comparator rhs) const;
    bool operator()(int actual, int required) const;
private:
    Id id;
};

}
