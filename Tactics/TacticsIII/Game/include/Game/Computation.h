#pragma once
#include <string>
#include <vector>
#include "Game/Operator.h"

namespace Game
{

struct Operation
{

    std::string description;
    Operator op;
    int value;
    int operator()(int value) const;
};

std::ostream& operator<<(std::ostream& s, const Operation& op);

class Computation
{
public:

    Computation() = default;
    Computation(std::string_view description, int value);
    virtual int Total() const;
    std::string Description() const;
    bool empty() const;
    Computation& append(Operator op, const Computation& o);
    Computation& operator+=(const Computation& o);
    Computation& operator-=(const Computation& o);
    Computation& operator*=(const Computation& o);
protected:
    Operation AsValue(Operator op) const;
    std::vector<Operation> operations;
};

Computation operator+(const Computation& a, const Computation& b);
Computation operator-(const Computation& a, const Computation& b);
Computation operator*(const Computation& a, const Computation& b);
Computation operator>=(const Computation& a, const Computation& b);

}
