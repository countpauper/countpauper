#include <GL/gl.h>
#include "Geometry/Line.h"
#include "Utility/Maths.h"
#include "Geometry/Vector.h"
#include "Geometry/AxisAlignedBoundingBox.h"
#include <sstream>

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
    Engine::Vector v = p - a;
    double length_squared = LengthSquared();
    if (length_squared == 0)
        return 0;   // project on point a==b
    return Vector(*this).Dot(v) / length_squared;
}


Line Line::Section(const Engine::Range<double>& range) const
{
    Range<double> slice = range / Length();
    slice &= Range(0.0, 1.0);
    auto v = Vector(*this);
    return Line(a + v * slice.begin, a + v * slice.end);
}


Line Line::Invert() const
{
    return Line{b, a};
}

Coordinate Line::Project(const Coordinate& p) const
{
    double interpolation_factor = ProjectionCoefficient(p);
    if (interpolation_factor <= 0)
    {
        return a;
    }
    else if (interpolation_factor >= 1)
    {
        return b;
    }
    else
    {
        return Coordinate::origin+Lerp(Vector(a), Vector(b), interpolation_factor);
    }
}

void Line::Render() const
{
    glBegin(GL_LINES);
        glVertex3d(a.x, a.y, a.z);
        glVertex3d(b.x, b.y, b.z);
    glEnd();
}
double Line::Distance(const Coordinate& p) const
{
    Coordinate projection = Project(p);
    return (p - projection).Length();
}


AABB Line::Bounds() const
{
    return AABB(a, b);
}

std::vector<std::pair<Position, double>> Line::Voxelize() const
{
    std::vector<std::pair<Position, double>> result;
    Coordinate pos = a;
    double remaining = Length();
    Position current(pos.x, pos.y, pos.z);
    while(true)
    {
        AABB bounds(Coordinate{
                    static_cast<double>(current.x),
                    static_cast<double>(current.y),
                    static_cast<double>(current.z)},
                Coordinate{
                    static_cast<double>(current.x+1),
                    static_cast<double>(current.y+1),
                    static_cast<double>(current.z+1)});
        if (bounds.Contains(b))
        {   // b in current voxel, add last voxel
            result.emplace_back(current, remaining);
            return result;
        }
        else
        {
            auto intersection = bounds.NamedIntersection(Invert());
            auto doubleCheckRange = bounds.Intersection(Invert());

            result.emplace_back(current, remaining - intersection.second);
            remaining = intersection.second;
            switch(intersection.first)
            {
                case Axis::NegZ:
                    current.z -=1;  // TODO Axis to position
                    break;
                case Axis::NegY:
                    current.y -=1;  // TODO Axis to position
                    break;
                case Axis::NegX:
                    current.x -=1;  // TODO Axis to position
                    break;
                case Axis::PosX:
                    current.x +=1;  // TODO Axis to position
                    break;
                case Axis::PosY:
                    current.y +=1;
                    break;
                case Axis::PosZ:
                    current.z +=1;
                    break;
            }
        }
    }
    return result;
}
bool Line::operator==(const Line& rhs) const
{
    return a == rhs.a && b == rhs.b;
}

Line& Line::operator*=(const Matrix& transformation)
{
    a *= transformation;
    b *= transformation;
    return *this;
}


Line& Line::operator*=(Quaternion q)
{
    auto va = q * Vector(a);
    a = Coordinate(va.x, va.y, va.z);
    auto vb = q * Vector(b);
    b = Coordinate(vb.x, vb.y, vb.z);
    return *this;
}

Line& Line::operator+=(Vector v)
{
    a += v;
    b += v;
    return *this;
}
Line& Line::operator-=(Vector v)
{
    a -= v;
    b -= v;
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
    s << line.a << "-" << line.b;
    return s;
}

}
