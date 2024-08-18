#pragma once

#include "Game/Stat.h"
#include "Game/Computation.h"
#include <vector>
#include <string>

namespace Game
{

class Requirement
{
public:
    enum Operator
    {
        not_equal = 0,
        equal = 1,
        less = 2,
        less_equal = 3,
        greater = 4,
        greater_equal = 5,
    };
    Requirement(Stat::Id stat, const Computation& actual, Operator op, const Computation& required);
    operator bool() const;
    Requirement operator!() const;
    std::string Description() const;
private:
    static std::string_view Describe(Operator op);
    Stat::Id stat;
    Operator op;
    Computation actual;
    Computation required;
};


class Requirements : public std::vector<Requirement>
{
public:
    Requirements() = default;
    Requirements(std::initializer_list<Requirement> init);

    operator bool() const;
};

}
