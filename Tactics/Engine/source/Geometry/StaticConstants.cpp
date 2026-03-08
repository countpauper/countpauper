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
const Vector Vector::X(1, 0, 0);
const Vector Vector::Y(0, 1, 0);
const Vector Vector::Z(0, 0, 1);

const Quaternion Quaternion::Identity(0, 0, 0, 1);

const Orientation Orientation::none{};
const Orientation Orientation::front{Orientation::Front};
const Orientation Orientation::back{Orientation::Back};
const Orientation Orientation::left{Orientation::Left};
const Orientation Orientation::right{Orientation::Right};
const Orientation Orientation::up{Orientation::Up};
const Orientation Orientation::down{Orientation::Down};

std::array<Engine::Position, 10> Orientation::vector =
{
    Engine::Position(0, 0, 0), // None
    Engine::Position(0, 0, 0), // None with extra steps
    Engine::Position(0, 1, 0), // Front
    Engine::Position(0, -1, 0), // Back
    Engine::Position(1, 0, 0), // Right
    Engine::Position(-1, 0, 0), // Left
    Engine::Position(),
    Engine::Position(),
    Engine::Position(0, 0, 1), // Up
    Engine::Position(0, 0,-1) // Down
};

const AxisAlignedPlane AxisAlignedPlane::null{Orientation::none, 0 };
const AxisAlignedPlane AxisAlignedPlane::xy{Orientation::up, 0 };
const AxisAlignedPlane AxisAlignedPlane::xz{Orientation::front, 0 };
const AxisAlignedPlane AxisAlignedPlane::yz{Orientation::right, 0 };

const Plane Plane::null{AxisAlignedPlane::null };
const Plane Plane::xy{AxisAlignedPlane::xy };
const Plane Plane::xz{AxisAlignedPlane::xz };
const Plane Plane::yz{AxisAlignedPlane::yz };

}
