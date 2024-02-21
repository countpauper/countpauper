#pragma once

#include "Geometry/Coordinate.h"
#include "Utility/Range.h"
#include "Geometry/Volume.h"

namespace Engine
{

    struct Vector;
    struct Line;
    struct Matrix;

    struct AABB : public IVolume, public Clone<AABB>
    {
        AABB() = default;
        AABB(const Coordinate& begin, const Coordinate& end);
        AABB(const Coordinate& begin, const Vector& extent);
        AABB(const Range<double>& x, const Range<double>& y, const Range<double>& z);
        AABB(const Line& line);

        Coordinate Begin() const;
        Coordinate End() const;
        Coordinate Center() const;

        Vector Extent() const;
        operator bool() const;
        Range<double> X() const;
        Range<double> Y() const;
        Range<double> Z() const;

        AABB GetBoundingBox() const override { return *this; }
        virtual double Distance(const Coordinate& p) const override;
        double Volume() const override;

        bool Contains(const Coordinate& p) const;
        Coordinate Clip(const Coordinate& p) const;
        double Intersection(const Line& line) const override;

        void Render();

        bool operator[](const Coordinate& p) const { return Contains(p); }
        AABB& operator|=(const Coordinate& p);
        AABB& operator&=(const AABB& p);
        AABB& operator|=(const AABB& p);
        AABB& Expand(const Engine::Vector& v);
        AABB& Grow(const Engine::Vector& v);
        AABB& Grow(double v);
        AABB& operator*=(const Matrix& transformation);
        AABB& operator+=(const Vector& offset);


        Range<double> x;
        Range<double> y;
        Range<double> z;

        static const AABB infinity; //= AABB(Range<double>::infinity(), Range<double>::infinity(), Range<double>::infinity());
        static const AABB empty; //= AABB((Range<double>::null(), Range<double>::null(), Range<double>::null());
    };


    AABB operator|(const AABB& a, const AABB& b);
    AABB operator|(const AABB& a, const Coordinate& c);
    AABB operator&(const AABB& a, const AABB& b);
    AABB operator*(const Matrix& transformation, const AABB& a);

} // ::Engine
