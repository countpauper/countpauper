#pragma once
#include <string>
#include <vector>
#include "Game/Operator.h"
#include "Utility/Range.h"

namespace Game
{

struct Operation
{

    std::string description;
    Operator op;
    int value;
    int operator()(int value) const;
    bool operator==(const Operation& o) const;
    bool Redundant() const;
};

std::ostream& operator<<(std::ostream& s, const Operation& op);

class Computation
{
public:
    Computation() = default;
    explicit Computation(Engine::Range<int> limit);
    explicit Computation(int value, std::string_view description="");
    virtual int Total() const;
    std::string Description() const;
    bool empty() const;
    explicit operator bool() const { return !empty(); }
    Computation& append(Operator op, const Computation& o);
    Computation& operator+=(const Computation& o);
    Computation& operator-=(const Computation& o);
    Computation& operator*=(const Computation& o);
    bool operator==(const Computation& o) const;
    Computation& Simplify();
protected:
    Operation AsValue(Operator op) const;
    Engine::Range<int> limit = Engine::Range<int>::max();
    std::vector<Operation> operations;
};

Computation operator+(const Computation& a, const Computation& b);
Computation operator-(const Computation& a, const Computation& b);
Computation operator*(const Computation& a, const Computation& b);
Computation operator>=(const Computation& a, const Computation& b);

}
