#pragma once

#include "Coordinate.h"
namespace Engine
{

    struct Vector;
    struct Range;

    struct AABB
    {
        AABB() = default;
        AABB(const Coordinate& p0, const Coordinate& p1);
        AABB(const Coordinate& p0, const Vector& extent);
        AABB(const Range& x, const Range& y, const Range& z);

        Vector Extent() const;
        double Volume() const;
        operator bool() const;
        Range X() const;
        Range Y() const;
        Range Z() const;


        Coordinate p0;
        Coordinate p1;
    };


    AABB operator|(const AABB& a, const AABB& b);
    AABB operator&(const AABB& a, const AABB& b);

} // ::Engine
