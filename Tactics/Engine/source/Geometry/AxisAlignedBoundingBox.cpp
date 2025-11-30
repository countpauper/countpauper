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
const AABB AABB::empty(Range<double>::null(), Range<double>::null(), Range<double>::null());

AABB::AABB(const Coordinate& begin, const Coordinate& end) :
    x(begin.x, end.x),
    y(begin.y, end.y),
    z(begin.z, end.z)
{
}

AABB::AABB(const Line& line) :
    AABB(line.a, line.b)
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
    return x[p.x] && y[p.y] && z[p.z];
}

Coordinate AABB::Clip(const Coordinate& p) const
{
    return Coordinate(x.Clip(p.x), y.Clip(p.y), z.Clip(p.z));
}

double ComputeCoefficient(double from, double to, double delta, bool neg)
{
    double v = to-from;
    // if (v*normal>0)
    //     return std::numeric_limits<double>::infinity();
    double coef = v / delta;
    if (coef<0.0)
        return std::numeric_limits<double>::infinity();
    else if (coef>1.0)
        return std::numeric_limits<double>::infinity();
    if (neg != std::signbit(v))
        return coef;
    else
        return -coef;
}

std::pair<Axis, double> AABB::NamedIntersection(const Line& line) const
{
    Vector V(line);

    if (std::abs(V.x) <= std::numeric_limits<double>::epsilon() && !x[line.a.x]) {
        return std::make_pair(Axis::None, std::numeric_limits<double>::infinity());
    }
    if (std::abs(V.y) <= std::numeric_limits<double>::epsilon() && !y[line.a.y]) {
        return std::make_pair(Axis::None, std::numeric_limits<double>::infinity());
    }
    if (std::abs(V.z) <= std::numeric_limits<double>::epsilon() && !z[line.a.z]) {
        return std::make_pair(Axis::None, std::numeric_limits<double>::infinity());
    }

    double coefficients[]{
        ComputeCoefficient(line.a.z, z.begin, V.z, true),
        ComputeCoefficient(line.a.y, y.begin, V.y, true),
        ComputeCoefficient(line.a.x, x.begin, V.x, true),
        std::numeric_limits<double>::quiet_NaN(),
        ComputeCoefficient(line.a.x, x.end, V.x, false),
        ComputeCoefficient(line.a.y, y.end, V.y, false),
        ComputeCoefficient(line.a.z, z.end, V.z, false),
    };
    double enter = -std::numeric_limits<double>::infinity();
    Axis enterAxis = Axis::None;
    double leave = - std::numeric_limits<double>::infinity();
    Axis leaveAxis = Axis::None;
    Axis axis = Axis::NegZ;

    for(double coef: std::span(coefficients))
    {
        if (coef>=0 && coef<=1.0 && coef > enter) {
            enterAxis = axis;
            enter = coef;
        }
        if (coef>=-1.0 && coef<=0 && coef > leave ) {
            leaveAxis = axis;
            leave = coef;
        }
        axis=(Axis)(int(axis)+1);
    }
    if (enterAxis==Axis::None)
    {
        return std::make_pair(leaveAxis, leave * V.Length());

    }
    return std::make_pair(enterAxis, enter * V.Length());
}

Range<double> AABB::Intersection(const Line& line) const
{
    Vector V(line);
    if (std::abs(V.x) <= std::numeric_limits<double>::epsilon() && !x[line.a.x]) {
        return Range<double>::empty();
    }
    if (std::abs(V.y) <= std::numeric_limits<double>::epsilon() && !y[line.a.y]) {
        return Range<double>::empty();
    }
    if (std::abs(V.z) <= std::numeric_limits<double>::epsilon() && !z[line.a.z]) {
        return Range<double>::empty();
    }

    double intersections[]{
        ComputeCoefficient(line.a.x, x.end, V.x, false),
        ComputeCoefficient(line.a.x, x.begin, V.x, true),

        ComputeCoefficient(line.a.y, y.begin, V.y, true),
        ComputeCoefficient(line.a.y, y.end, V.y, false),

        ComputeCoefficient(line.a.z, z.begin, V.z, true),
        ComputeCoefficient(line.a.z, z.end, V.z, false),
    };
    double enter = 0.0; // at least at coefficient of A
    double leave = 1.0; // maximum at coeffiecient of B

    for(auto intersect : std::span(intersections))
    {
        if (std::isinf(intersect) || std::isnan(intersect)) {
            continue;
        }
        if ((intersect<=0) && (-intersect<leave)) {
            leave = -intersect; // leave at the smallest negative (intersect from back of plane)
        }
        if ((intersect>=0) && (intersect>enter))
            enter = intersect;  // enter at the smallest positive (intersect from front of plane)
    }
    return Range<double>(enter * line.Length(), leave * line.Length());
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
    vertex[1] = Vector(minC.x, maxC.y, minC.z);
    vertex[2] = Vector(maxC.x, maxC.y, minC.z);
    vertex[3] = Vector(maxC.x, minC.y, minC.z);
    vertex[4] = Vector(minC.x, minC.y, maxC.z);
    vertex[5] = Vector(minC.x, maxC.y, maxC.z);
    vertex[6] = Vector(maxC);
    vertex[7] = Vector(maxC.x, minC.y, maxC.z);
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
        vertex[i] = Begin() + Vector(i & 1 ? extent.x : 0,
            i & 2 ? extent.y : 0,
            i & 4 ? extent.z : 0);
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
    x += offset.x;
    y += offset.y;
    z += offset.z;
    return *this;
}

AABB::operator bool() const
{
    return Volume()!=0;
}

double AABB::Volume() const
{
    Vector extent = Extent();
    return extent.x * extent.y * extent.z;
}

double AABB::Distance(const Coordinate& p) const
{
    if (GetBoundingBox().Contains(p))
    {
        Vector d(std::min(p.x - x.begin, x.end - p.x),
            std::min(p.y - y.begin, y.end - p.y),
            std::min(p.z - z.begin, z.end - p.z));
        return -std::min(std::min(d.x, d.y), d.z);
    }
    else
    {
        Vector d(std::max(0.0, std::max(x.begin - p.x, p.x - x.end)),
            std::max(0.0, std::max(y.begin - p.y, p.y - y.end)),
            std::max(0.0, std::max(z.begin - p.z, p.z - z.end)));
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


Plane AABB::Right() const
{   // +X
    return Plane{Vector::X, -x.end};
}


Plane AABB::Left() const
{   // -X
    return Plane{-Vector::X, x.begin};
}

Plane AABB::Front() const
{   // +Y
    return Plane{Vector::Y, -y.end};
}

Plane AABB::Back() const
{   // -Y
    return Plane{-Vector::Y, y.begin};
}

Plane AABB::Top() const
{   // +Z
    return Plane{Vector::Z, -z.end};
}

Plane AABB::Bottom() const
{   // -Z
    return Plane{-Vector::Z, z.begin};
}



AABB& AABB::operator|=(const Coordinate& p)
{
    x |= p.x;
    y |= p.y;
    z |= p.z;
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
    x.Expand(v.x);
    y.Expand(v.y);
    z.Expand(v.z);
    return *this;
}

AABB& AABB::Grow(double v)
{
    return Grow(Vector(v, v, v));
}

AABB& AABB::Grow(const Engine::Vector& v)
{
    x.Grow(v.x);
    y.Grow(v.y);
    z.Grow(v.z);
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
