#include "stdafx.h"
#include "Game/Anatomy.h"
#include "Game/Direction.h"

namespace Game
{
std::wistream& operator>>(std::wistream& s, Plane& plane)
{
    std::wstring label;
    const static std::map<std::wstring, Plane> planeMap({
        { L"Left", Plane::Left },
        { L"Right", Plane::Right },
        { L"Coronal", Plane::Coronal },
        { L"Front", Plane::Front },
        { L"Back", Plane::Back },
        { L"Sagital", Plane::Sagittal },
        { L"Around", Plane::Around },
        { L"Top", Plane::Top },
        { L"Bottom", Plane::Bottom },
        { L"Transversal", Plane::Transversal },
        { L"Bottom|Top", Plane::Transversal },
        { L"Around", Plane::Around },
        { L"All", Plane::All }
    });
    s >> label;
    plane = planeMap.at(label);
    return s;
}

Anatomy::Anatomy(Plane plane, int height) :
    plane(plane),
    height(height)
{
}

// Striking plane of actor
Anatomy::Anatomy(Trajectory trajectory, int height) :
Anatomy(ToPlane(trajectory), height)
{
}

// facing plane of target
Anatomy::Anatomy(const Direction& facing, const Direction& target, int height) :
    Anatomy(Facing(target, facing), height)
{
}

// transform planes
Anatomy::Anatomy(Anatomy attack, Anatomy target) :
    Anatomy(Transform(attack.plane, target.plane), target.height - attack.height)
{
}

bool Anatomy::Match(const Anatomy other) const
{
    if (height != other.height)
        return false;
    if ((unsigned(plane) & unsigned(other.plane)) == 0)
        return false;
    return true;
}


Plane ToPlane(Trajectory trajectory)
{
    switch (trajectory)
    {
    case Trajectory::Straight:
        return Plane::Front;
    case Trajectory::Reverse:
        return Plane::Back;
    case Trajectory::Forehand:
        return Plane::Right;
    case Trajectory::Backhand:
        return Plane::Left;
    case Trajectory::Down:
        return Plane::Top;
    case Trajectory::Up:
        return Plane::Bottom;
    default:
        assert(false);  // don't know how to aim 
        return Plane::None;
    }
}

Plane Transform(Plane from, Plane to)
{
    switch (to)
    {
    case Plane::Front:
        return Front(from);
    case Plane::Back:
        return Back(from);
    case Plane::Right:
        return Right(from);
    case Plane::Left:
        return Left(from);
    case Plane::Top:
        assert(false); // 3d attack not entirely implemented
        return Plane::Top;
    case Plane::Bottom:
        assert(false); // 3d attack not entirely implemented
        return Plane::Bottom;
    default:
        return Plane::None;
    }
}


Plane Back(Plane from)
{
    switch (from)
    {
    case Plane::Front:
        return Plane::Back;
    case Plane::Back:
        return Plane::Front;
    default:
        return from;
    }
}
Plane Front(Plane from)
{
    switch (from)
    {
    case Plane::Left:
        return Plane::Right;
    case Plane::Right:
        return Plane::Left;
    default:
        return from;
    }
}

Plane Left(Plane from)
{
    switch (from)
    {
    case Plane::Front:
        return Plane::Left;
    case Plane::Back:
        return Plane::Right;
    case Plane::Left:
        return Plane::Front;
    case Plane::Right:
        return Plane::Back;
    default:
        return from;
    }
}

Plane Right(Plane from)
{
    switch (from)
    {
    case Plane::Front:
        return Plane::Right;
    case Plane::Back:
        return Plane::Left;
    case Plane::Left:
        return Plane::Back;
    case Plane::Right:
        return Plane::Front;
    default:
        return from;
    }
}

Plane Facing(Direction direction, Direction facing)
{
    if (facing == direction)
        return Plane::Back;
    if (facing.Opposite(direction))
        return Plane::Front;
    if (facing.Clockwise(direction))
        return Plane::Right;
    else if (facing.CounterClockwise(direction))
        return Plane::Left;
    else
        return Plane::None;
}

std::wistream& operator>>(std::wistream& s, Anatomy& v)
{
    s >> v.plane >> v.height;
    return s;
}
}    // ::Game
