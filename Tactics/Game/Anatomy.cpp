#include "stdafx.h"
#include "Game/Anatomy.h"
#include <sstream>
#include "Engine/Utils.h"
#include "Engine/from_string.h"
#include "Game/Direction.h"

namespace Game
{
    const static std::map<std::wstring, Plane> planeMap({
        { L"Left", Plane::Left },
        { L"Right", Plane::Right },
        { L"Coronal", Plane::Coronal },
        { L"Front", Plane::Front },
        { L"Back", Plane::Back },
        { L"Sagittal", Plane::Sagittal },
        { L"Around", Plane::Around },
        { L"Top", Plane::Top },
        { L"Bottom", Plane::Bottom },
        { L"Transversal", Plane::Transversal },
        { L"Bottom|Top", Plane::Transversal },
        { L"Around", Plane::Around },
        { L"All", Plane::All }
    });

std::wistream& operator>>(std::wistream& s, Plane& plane)
{
    std::wstring label;
    s >> label;
    plane = planeMap.at(label);
    return s;
}

std::ostream& operator<<(std::ostream& s, const Plane& plane)
{
    auto it = std::find_if(planeMap.begin(), planeMap.end(), [plane](const decltype(planeMap)::value_type& plane_pair)
    {
        return plane_pair.second == plane;
    });
    assert(it!=planeMap.end()); // plane not in map
    s << Engine::from_string<std::string>(L"test").c_str();
    return s;
}

Anatomy::Anatomy() :
    plane(Plane::None),
    position(0),
    size(0)
{
}

Anatomy::Anatomy(Plane plane, unsigned pos, unsigned size) :
    plane(plane),
    position(pos),
    size(size)
{
}

// Striking plane of actor
Anatomy::Anatomy(Trajectory trajectory, unsigned position, unsigned range) :
    Anatomy(ToPlane(trajectory), position, range)
{
}

// facing plane of target
Anatomy::Anatomy(const Direction& facing, const Direction& target, int position) :
    Anatomy(Facing(target, facing), position)
{
}

// transform planes
Anatomy::Anatomy(Anatomy attack, Anatomy target) :
Anatomy(Transform(attack.plane, target.plane), attack.position-target.position, attack.size)
{
    assert(target.position == 0);   // height difference not properly implemented, needs clipping or sign
}

bool Anatomy::Contains(const Anatomy& other) const
{
    if (position >= other.position + other.size)
        return false;
    if (position + size <= other.position)
        return false;
    if ((unsigned(plane) & unsigned(other.plane)) == 0)
        return false;
    return true;
}

Anatomy::operator bool() const
{
    return plane != Plane::None && size > 0;
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
    std::wstring str;
    s >> str;
    
    auto heightStart= str.find(L'[');
    if (heightStart == std::wstring::npos)
        throw std::runtime_error("Missing [ in anatomy");
    auto planeStr = str.substr(0, heightStart);
    heightStart += 1;
    auto heightEnd = str.find(L']');
    if (heightStart == std::wstring::npos)
        throw std::runtime_error("Missing ] in anatomy");
    if (heightStart >= heightEnd)
        throw std::runtime_error("No height between [] in anatomy");
    v.plane = planeMap.at(planeStr);
    auto heightStr = str.substr(heightStart, heightEnd - heightStart);
    auto rangeDelimiter = heightStr.find(L'-');
    if (rangeDelimiter == std::wstring::npos)
    {
        v.position = Engine::from_string<unsigned>(heightStr);
        v.size = 1;
    }
    else
    {
        auto positionStr = heightStr.substr(0, rangeDelimiter);
        auto sizeStr = heightStr.substr(rangeDelimiter + 1);
        v.position = Engine::from_string<unsigned>(positionStr);
        auto end = Engine::from_string<unsigned>(sizeStr);
        v.size = end+1 - v.position;
    }
    return s;
}

}    // ::Game
