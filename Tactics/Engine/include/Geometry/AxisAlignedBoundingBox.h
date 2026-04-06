#pragma once

#include "Geometry/Coordinate.h"
#include "Utility/Range.h"
#include "Geometry/Volume.h"
#include "Geometry/AxisAlignedPlane.h"
#include "Geometry/Orientation.h"

namespace Engine
{

    struct Vector;
    struct Line;
    struct Matrix;
    class Orientation;

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
        bool IsEmpty() const;
        explicit inline operator bool() const { return !IsEmpty(); }

        Range<double> X() const;
        Range<double> Y() const;
        Range<double> Z() const;
        Range<double> Extent(const Orientation& axis) const;

        AABB GetBoundingBox() const override { return *this; }
        virtual double Distance(const Coordinate& p) const override;
        double Volume() const override;

        bool Contains(const Coordinate& p) const;
        Coordinate Clip(const Coordinate& p) const;
        std::pair<Orientation, double> Entry(const Line& line) const;
        std::pair<Orientation, double> Exit(const Line& line) const;
        Range<double> Intersection(const Line& line) const override;

        void Render();

        AxisAlignedPlane Side(Orientation side) const;
        AxisAlignedPlane Right() const;    // +X
        AxisAlignedPlane Left() const;     // -X
        AxisAlignedPlane Front() const;    // +Y
        AxisAlignedPlane Back() const;     // -Y
        AxisAlignedPlane Top() const;      // +Z
        AxisAlignedPlane Bottom() const;   // -Z

        bool operator[](const Coordinate& p) const { return Contains(p); }
        AABB& operator|=(const Coordinate& p);
        AABB& operator&=(const AABB& p);
        AABB& operator|=(const AABB& p);
        AABB& Expand(const Engine::Vector& v);
        AABB& Grow(const Engine::Vector& v);
        AABB& Grow(double v);
        AABB& operator*=(const Matrix& transformation);
        AABB& operator+=(const Vector& offset);
        bool operator==(const AABB& other) const;

        Range<double> x;
        Range<double> y;
        Range<double> z;

        static const AABB infinity; //= AABB(Range<double>::infinity(), Range<double>::infinity(), Range<double>::infinity());
        static const AABB empty; //= AABB((Range<double>::empty(), Range<double>::empty(), Range<double>::empty());
    };


    AABB operator|(const AABB& a, const AABB& b);
    AABB operator|(const AABB& a, const Coordinate& c);
    AABB operator&(const AABB& a, const AABB& b);
    AABB operator*(const Matrix& transformation, const AABB& a);

} // ::Engine
