#include "Geometry/AxisAlignedBoundingBox.h"
#include "Geometry/Vector.h"
#include "Utility/Range.h"
#include "Geometry/Line.h"
#include "Geometry/Matrix.h"
#include "Rendering/Drawing.h"
#include "Utility/GLUtil.h"
#include <array>
#include <GL/gl.h>
#include <cassert>

namespace Engine
{


const AABB AABB::infinity(Range<double>::infinity(), Range<double>::infinity(), Range<double>::infinity());
const AABB AABB::empty(Range<double>::empty(), Range<double>::empty(), Range<double>::empty());

AABB::AABB(const Coordinate& begin, const Coordinate& end) :
    x(begin.X(), end.X()),
    y(begin.Y(), end.Y()),
    z(begin.Z(), end.Z())
{
}

AABB::AABB(const Line& line) :
    AABB(line.A(), line.B())
{
}

AABB::AABB(const Coordinate& begin, const Vector& extent) :
    AABB(begin, begin + extent)
{
}

AABB::AABB(const Range<double>& x, const Range<double>& y, const Range<double>& z) :
    x(x),
    y(y),
    z(z)
{
}

Coordinate AABB::Begin() const
{
    return Coordinate(x.begin, y.begin, z.begin);
}
Coordinate AABB::End() const
{
    return Coordinate(x.end, y.end, z.end);
}

Coordinate AABB::Center() const
{
    return Coordinate(x.Middle(), y.Middle(), z.Middle());

}
Vector AABB::Extent() const
{
    return Vector(x.Size(), y.Size(), z.Size());
}

bool AABB::Contains(const Coordinate& p) const
{
    for (auto axis : Orientations::axes) {
        if (!Extent(axis)[p.Axis(axis)])
            return false;
    }
    return true;
}

Coordinate AABB::Clip(const Coordinate& p) const
{
    return Coordinate(x.Clip(p.X()), y.Clip(p.Y()), z.Clip(p.Z()));
}

std::pair<bool, double> ComputeCoefficient(double from, double to, double delta, double normal)
{
    double v = to-from;
    // if (v*normal>0)
    //     return std::numeric_limits<double>::infinity();
    double coef = v / delta;
    return std::make_pair( std::signbit(v) != std::signbit(normal), coef);
}

bool is_closer(double a, double b)
{
    return std::abs(a) < std::abs(b);
}

double closest_number(double a, double b)
{
    if (std::isnan(a))
        return b;
    if (std::isnan(b))
        return a;
    if (is_closer(a,b))
        return a;
    return b;
}

double furthest_number(double a, double b)
{
    if (std::isnan(a))
        return b;
    if (std::isnan(b))
        return a;
    if (is_closer(a,b))
        return b;
    return a;
}


std::pair<Orientation, double> AABB::Entry(const Line& line) const
{
    Vector V(line);
    Orientation entrySide;
    Range<double> rng(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN());
    for(auto axis : Orientations::axes)
    {
        auto intersection = Side(axis).Intersection(line);
        if (!intersection)
        {
            if (!Extent(axis)[line.o.Axis(axis)])
                return std::make_pair(Orientation::none, std::numeric_limits<double>::quiet_NaN());
        }
        else
        {
            auto opposite = Side(-axis).Intersection(line);
            Range<double> t; // begin = t-close, end = t-far
            Orientation enterSide;
            if (intersection.GetNumber() <= opposite.GetNumber())
            {
                t.begin = intersection.GetNumber();
                t.end = opposite.GetNumber();
                enterSide = axis;
            }
            else
            {
                t.begin = opposite.GetNumber();
                t.end = intersection.GetNumber();
                enterSide = -axis;
            }
            if (!(t.begin < rng.begin)) // rng.begin can be NaN so double negative
            {
                rng.begin = t.begin;
                entrySide = enterSide;
            }
            rng.end = std::min(t.end, rng.end);
        }
    }
    if (rng.empty())
    {
        return std::make_pair(Orientation::none, std::numeric_limits<double>::quiet_NaN());
    }
    return std::make_pair(entrySide, rng.begin);
}


std::pair<Orientation, double> AABB::Exit(const Line& line) const
{
    Vector V(line);
    Orientation exitSide;
    Range<double> rng(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN());
    for(auto axis : Orientations::axes)
    {
        auto intersection = Side(axis).Intersection(line);
        if (!intersection)
        {
            if (!Extent(axis)[line.o.Axis(axis)])
                return std::make_pair(Orientation::none, std::numeric_limits<double>::quiet_NaN());
        }
        else
        {
            auto opposite = Side(-axis).Intersection(line);
            Range<double> t; // begin = t-close, end = t-far
            Orientation exSide;
            if (intersection.GetNumber() >= opposite.GetNumber())
            {
                t.end = intersection.GetNumber();
                t.begin = opposite.GetNumber();
                exSide = axis;
            }
            else
            {
                t.end = opposite.GetNumber();
                t.begin = intersection.GetNumber();
                exSide = -axis;
            }
            if (!(t.end > rng.end)) // rng.end can be NaN so double negative
            {
                rng.end = t.end;
                exitSide = exSide;
            }
            rng.begin = std::max(t.begin, rng.begin);
        }
    }
    if (rng.empty())
    {
        return std::make_pair(Orientation::none, std::numeric_limits<double>::quiet_NaN());
    }
    return std::make_pair(exitSide, rng.end);
}



Range<double> AABB::Intersection(const Line& line) const
{
    // https://en.wikipedia.org/wiki/Slab_method

    Vector V(line);
    Range<double> result(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN());
    for(auto axis : Orientations::axes)
    {
        auto intersection = Side(axis).Intersection(line);
        if (!intersection)
        {
            if (!Extent(axis)[line.o.Axis(axis)])
                return Range<double>::empty();
        }
        else
        {
            auto opposite = Side(-axis).Intersection(line);
            Range<double> t; // begin = t-close, end = t-far
            t.begin = std::min(intersection.GetNumber(), opposite.GetNumber());
            t.end  = std::max(intersection.GetNumber(), opposite.GetNumber());
            result.begin = std::max(t.begin, result.begin);
            result.end = std::min(t.end, result.end);
        }
    }
    return result;
}


void AABB::Render()
{
    Coordinate minC = Begin();
    Coordinate maxC = End();
    auto blend = glTemporary::Disable(GL_BLEND);
    glPushMatrix();
    glLoadIdentity();
    std::array<Vector, 8> vertex;
    vertex[0] = Vector(minC);
    vertex[1] = Vector(minC.X(), maxC.Y(), minC.Z());
    vertex[2] = Vector(maxC.X(), maxC.Y(), minC.Z());
    vertex[3] = Vector(maxC.X(), minC.Y(), minC.Z());
    vertex[4] = Vector(minC.X(), minC.Y(), maxC.Z());
    vertex[5] = Vector(minC.X(), maxC.Y(), maxC.Z());
    vertex[6] = Vector(maxC);
    vertex[7] = Vector(maxC.X(), minC.Y(), maxC.Z());
    glBegin(GL_LINE_LOOP);
        glVertex(vertex[0]);
        glVertex(vertex[1]);
        glVertex(vertex[2]);
        glVertex(vertex[3]);
    glEnd();
    glBegin(GL_LINE_LOOP);
        glVertex(vertex[4]);
        glVertex(vertex[5]);
        glVertex(vertex[6]);
        glVertex(vertex[7]);
    glEnd();
    glBegin(GL_LINES);
        glVertex(vertex[0]);
        glVertex(vertex[4]);
        glVertex(vertex[1]);
        glVertex(vertex[5]);
        glVertex(vertex[2]);
        glVertex(vertex[6]);
        glVertex(vertex[3]);
        glVertex(vertex[7]);
    glEnd();
    glPopMatrix();
}

AABB& AABB::operator*=(const Matrix& transformation)
{
    // wrong
    //auto begin = transformation * Begin();
    //auto end = transformation * End();
        //lame/slow way is to rotate all 8 corners and put a bounding box around all of them
        // reference imlementation
    std::array<Coordinate, 8> vertex;
    auto extent = Extent();
    for (int i = 0; i < 8; i++)
    {
        vertex[i] = Begin() + Vector(i & 1 ? extent.X() : 0,
            i & 2 ? extent.Y() : 0,
            i & 4 ? extent.Z() : 0);
        vertex[i] *= transformation;
    }
    x = y = z = Range<double>(std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity());
    for (const auto& v : vertex)
    {
        (*this) |= v;
    }

        // could also transform the center and then the half extent vector minus the translation, then mirror it 8 ways. Still get 8 points that need minned and maxes
        // there must be something about the orientation/scale 3x3 matrix if all 3 aligned axes are transformed by it. Only need their length really to get a new oriented bounding box
        // the sin of the angle of the matrix axes says something about the size factor of the original. 0 & 90 is the same (but other axis).
        // Take the extent and tranform it with the 3x3?
/*
// This is also not exactly right with the
    Coordinate newCenter = Center() + transformation.Translation();
    Matrix rotscale(transformation.X(), transformation.Y(), transformation.Z(),Vector());
    Vector newExtent = rotscale * Extent();
    *this = AABB(newCenter -newExtent * 0.5, newExtent);
*/

    return *this;
}

AABB& AABB::operator+=(const Vector& offset)
{
    x += offset.X();
    y += offset.Y();
    z += offset.Z();
    return *this;
}

bool AABB::operator==(const AABB& other) const
{
    return x == other.x && y == other.y && z == other.z;
}

bool AABB::IsEmpty() const
{
    return x.IsEmpty() && y.IsEmpty() && z.IsEmpty();
}
double AABB::Volume() const
{
    Vector extent = Extent();
    return extent.X() * extent.Y() * extent.Z();
}

double AABB::Distance(const Coordinate& p) const
{
    auto [ px, py, pz] = p.Coefficients();

    if (GetBoundingBox().Contains(p))
    {
        Vector d(std::min(px - x.begin, x.end - px),
            std::min(py - y.begin, y.end - py),
            std::min(pz - z.begin, z.end - pz));
        return -std::min(std::min(d.X(), d.Y()), d.Z());
    }
    else
    {
        Vector d(std::max(0.0, std::max(x.begin - px, px - x.end)),
            std::max(0.0, std::max(y.begin - py, py - y.end)),
            std::max(0.0, std::max(z.begin - pz, pz - z.end)));
        return d.Length();
    }
}

Range<double> AABB::X() const
{
    return x;
}

Range<double> AABB::Y() const
{
    return y;
}

Range<double> AABB::Z() const
{
    return z;
}

Range<double> AABB::Extent(const Orientation& axis) const
{
    if (axis.IsX())
        return X();
    else if (axis.IsY())
        return Y();
    else if (axis.IsZ())
        return Z();
    else
        return Range<double>::empty();
}

AxisAlignedPlane AABB::Side(Orientation side) const
{
    if (!side)
        return AxisAlignedPlane(side);

    const Range<double>& axisRange =
        side.IsX() ? this->x :
        side.IsY() ? this->y :
        this->z;
    if (side.IsNegative())
        return AxisAlignedPlane(side, axisRange.begin);
    else
        return AxisAlignedPlane(side, axisRange.end);
}

AABB& AABB::operator|=(const Coordinate& p)
{
    x |= p.X();
    y |= p.Y();
    z |= p.Z();
    return *this;
}

AABB& AABB::operator&=(const AABB& o)
{
    *this = (*this) & o;
    return *this;
}

AABB& AABB::operator|=(const AABB& o)
{
    *this = (*this) | o;
    return *this;
}


AABB& AABB::Expand(const Engine::Vector& v)
{
    x.Expand(v.X());
    y.Expand(v.Y());
    z.Expand(v.Z());
    return *this;
}

AABB& AABB::Grow(double v)
{
    return Grow(Vector(v, v, v));
}

AABB& AABB::Grow(const Engine::Vector& v)
{
    x.Grow(v.X());
    y.Grow(v.Y());
    z.Grow(v.Z());
    return *this;
}

AABB operator|(const AABB& a, const AABB& b)
{
    return AABB(
        a.X() | b.X(),
        a.Y() | b.Y(),
        a.Z() | b.Z());
}

AABB operator|(const AABB& a, const Coordinate& c)
{
    return AABB(a) |= c;
}

AABB operator&(const AABB& a, const AABB& b)
{
    return AABB(
        a.X() & b.X(),
        a.Y() & b.Y(),
        a.Z() & b.Z());
}

AABB operator*(const Matrix& transformation, const AABB& a)
{
    return AABB(a) *= transformation;
}


}
