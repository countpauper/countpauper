#pragma once
#include <string>
#include <vector>

namespace Game
{

class Computation
{
public:
    Computation() = default;
    Computation(std::string_view description, int value);
    virtual int Total() const;
    std::string Description() const;
    bool empty() const;
    Computation& operator+=(const Computation& o);
    Computation& operator-=(const Computation& o);
protected:
    std::vector<std::pair<std::string_view, int>> contributions;
};

Computation operator+(const Computation& a, const Computation& b);
Computation operator-(const Computation& a, const Computation& b);

}
