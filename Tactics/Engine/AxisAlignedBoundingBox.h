#pragma once

#include "Coordinate.h"
#include "Range.h"
namespace Engine
{

    struct Vector;

    struct AABB
    {
        AABB() = default;
        AABB(const Coordinate& p0, const Coordinate& p1);
        AABB(const Coordinate& p0, const Vector& extent);
        AABB(const Range<double>& x, const Range<double>& y, const Range<double>& z);

        Vector Extent() const;
        double Volume() const;
        operator bool() const;
        Range<double> X() const;
        Range<double> Y() const;
        Range<double> Z() const;

        AABB& operator|=(const Coordinate& p);

        Range<double> x;
        Range<double> y;
        Range<double> z;
    };


    AABB operator|(const AABB& a, const AABB& b);
    AABB operator&(const AABB& a, const AABB& b);

} // ::Engine
