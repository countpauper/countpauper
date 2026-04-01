// statics can have a dependency on each other, but undefined compilation order so they are collected here
#include "Geometry/Coordinate.h"
#include "Geometry/IntBox.h"
#include "Geometry/Plane.h"
#include "Geometry/Vector.h"
#include "Geometry/Quaternion.h"
#include "Geometry/AxisAlignedPlane.h"
#include "Geometry/Orientation.h"

namespace Engine
{

const Coordinate Coordinate::origin { 0,0,0 };

const IntBox IntBox::empty(Range<int>(0,0), Range<int>(0,0), Range<int>(0,0));
const IntBox IntBox::all(Range<int>(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()),
    Range<int>(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()),
    Range<int>(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()));


const Vector Vector::zero(0, 0, 0);
const Vector Vector::XAxis(1, 0, 0);
const Vector Vector::YAxis(0, 1, 0);
const Vector Vector::ZAxis(0, 0, 1);

const Quaternion Quaternion::Identity(0, 0, 0, 1);

const RightAngle RightAngle::straight{0};
const RightAngle RightAngle::clockwise{-1};
const RightAngle RightAngle::counter_clockwise{1};
const RightAngle RightAngle::around{2};

const Orientation Orientation::none{};
const Orientation Orientation::up{Orientation::Up};
const Orientation Orientation::down{Orientation::Down};
const Orientation Orientation::front{Orientation::Front};
const Orientation Orientation::back{Orientation::Back};
const Orientation Orientation::right{Orientation::Right};
const Orientation Orientation::left{Orientation::Left};


const AxisAlignedPlane AxisAlignedPlane::null{Orientation::none, 0 };
const AxisAlignedPlane AxisAlignedPlane::xy{Orientation::up, 0 };
const AxisAlignedPlane AxisAlignedPlane::xz{Orientation::front, 0 };
const AxisAlignedPlane AxisAlignedPlane::yz{Orientation::right, 0 };

const Plane Plane::null{AxisAlignedPlane::null };
const Plane Plane::xy{AxisAlignedPlane::xy };
const Plane Plane::xz{AxisAlignedPlane::xz };
const Plane Plane::yz{AxisAlignedPlane::yz };

}
