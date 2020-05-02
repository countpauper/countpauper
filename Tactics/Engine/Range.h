#pragma once
#pragma once

#include "Coordinate.h"
namespace Engine
{

struct Range
{
    Range() = default;
    Range(double begin, double end);

    operator bool() const;
    double Size() const;

    double begin;
    double end;
};

Range operator|(const Range& a, const Range& b);
Range operator&(const Range& a, const Range& b);


}
