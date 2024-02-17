#include "Coordinate.h"
#include "Range.h"
#pragma once

namespace Engine
{

struct AABB;

struct  Line
{
public:
    Coordinate a;
    Coordinate b;
    Line() = default;
    Line(const Coordinate& a, const Coordinate& b) :
        a(a),
        b(b)
    {
    }
    double LengthSquared() const;
    double Length() const;
    double Distance(const Coordinate& p) const;
    double ProjectionCoefficient(const Coordinate& p) const;
    Line Section(const Engine::Range<double>& range) const;
    Coordinate Project(const Coordinate& p) const;
    void Render() const;
    AABB Bounds() const;
    bool operator==(const Line& rhs) const;
    Line& operator*=(const Matrix& transformation);
    Line& operator*=(Quaternion q);
    Line& operator+=(Vector v);
    Line& operator-=(Vector v);
};


Line operator*(const Matrix& transformation, Line line);
Line operator*(Quaternion q, Line line);
Line operator+(Line line, Vector v);
Line operator-(Line line, Vector v);

std::ostream& operator<<(std::ostream& s, const Line& line);

}
