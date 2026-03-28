#include <GL/gl.h>
#include "Geometry/Line.h"
#include "Utility/Maths.h"
#include "Geometry/Vector.h"
#include "Geometry/AxisAlignedBoundingBox.h"
#include "Rendering/Drawing.h"
#include <sstream>
#include <cassert>

namespace Engine
{

double Line::LengthSquared() const
{
    return Vector(*this).LengthSquared();
}

double Line::Length() const
{
    return Vector(*this).Length();
}

double Line::ProjectionCoefficient(const Coordinate& p) const
{
    Engine::Vector v = p - o;
    double length_squared = LengthSquared();
    if (length_squared == 0)
        return 0;   // project on point a==b
    return Vector(*this).Dot(v) / length_squared;
}


Line Line::Section(const Engine::Range<double>& range) const
{
    Range<double> slice = range;
    slice &= Range(0.0, 1.0);
    auto v = Vector(*this);
    return Line(o + v * slice.begin, o + v * slice.end);
}


Line Line::Invert() const
{
    return Line{B(), A()};
}

Coordinate Line::Project(const Coordinate& p) const
{
    double interpolation_factor = ProjectionCoefficient(p);
    if (interpolation_factor <= 0)
    {
        return A();
    }
    else if (interpolation_factor >= 1)
    {
        return B();
    }
    else
    {
        return Coordinate::origin+Lerp(Vector(A()), Vector(B()), interpolation_factor);
    }
}

void Line::Render() const
{
    glBegin(GL_LINES);
        glVertex(A());
        glVertex(B());
    glEnd();
}
double Line::Distance(const Coordinate& p) const
{
    Coordinate projection = Project(p);
    return (p - projection).Length();
}


AABB Line::Bounds() const
{
    return AABB(A(), B());
}

std::vector<std::pair<Position, double>> Line::Voxelize() const
{
    std::vector<std::pair<Position, double>> result;
    Position current(o.X(), o.Y(), o.Z());
    double previous = 0.0;
    while(previous<1.0)
    {
        AABB bounds(Coordinate{
                    static_cast<double>(current.x),
                    static_cast<double>(current.y),
                    static_cast<double>(current.z)},
                    Vector(1,1,1));
        auto intersection = bounds.Exit(*this);
        if (!intersection.first) {
            assert(previous==0.0);  // does not leave the start ? at least it starts on an edge and stays outside
            result.emplace_back(current, 1.0 - previous);
            break;
        }
        assert(intersection.first); // somehow left the line. could probably continue; to skip edge cases, but which way?
        result.emplace_back(current, std::min(1.0, intersection.second) - previous);
        current += intersection.first.GetVector();
        previous = intersection.second;
    }
    return result;
}
bool Line::operator==(const Line& rhs) const
{
    return o == rhs.o && v == rhs.v;
}

Line& Line::operator*=(const Matrix& transformation)
{
    return *this = Line(transformation * A(), transformation * B());
}


Line& Line::operator*=(Quaternion q)
{
    auto va = q * Vector(A());
    auto vb = q * Vector(B());
    return *this = Line(Coordinate(va.X(), va.Y(), va.Z()), Coordinate(vb.X(), vb.Y(), vb.Z()));
}

Line& Line::operator+=(Vector v)
{
    o += v;
    return *this;
}
Line& Line::operator-=(Vector v)
{
    o -= v;
    return *this;
}


Line operator*(const Matrix& transformation, Line line)
{
    return line*=transformation;
}

Line operator*(Quaternion q, Line line)
{
    return line *= q;
}

Line operator+(Line line, Vector v)
{
    return line+=v;
}

Line operator-(Line line, Vector v)
{
    return line -= v;
}


std::ostream& operator<<(std::ostream& s, const Line& line)
{
    s << line.A() << "-" << line.B();
    return s;
}

}
