// statics can have a dependency on each other, but undefined compilation order so they are collected here
#include "Geometry/Coordinate.h"
#include "Geometry/IntBox.h"
#include "Geometry/Plane.h"
#include "Geometry/Vector.h"
namespace Engine
{

const Coordinate Coordinate::origin { 0,0,0 };

const IntBox IntBox::empty(Range<int>(0,0), Range<int>(0,0), Range<int>(0,0));
const IntBox IntBox::all(Range<int>(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()),
    Range<int>(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()),
    Range<int>(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()));


const Vector Vector::zero(0, 0, 0);
const Vector Vector::X(1, 0, 0);
const Vector Vector::Y(0, 1, 0);
const Vector Vector::Z(0, 0, 1);

Plane Plane::null(Vector::zero, 0);
Plane Plane::xy(Vector::Z, 0);
Plane Plane::xz(Vector::Y, 0);
Plane Plane::yz(Vector::X, 0);

}
