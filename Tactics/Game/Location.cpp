#include "stdafx.h"
#include "Game/Location.h"
#include "Engine/from_string.h"

namespace Game
{

Location::Location() :
    position(0),
    size(0)
{
}

Location::Location(const Physics::Plane& plane, unsigned pos, unsigned size) :
    plane(plane),
    position(pos),
    size(size)
{
}

bool Location::Contains(const Location& other) const
{
    if (position >= other.position + other.size)
        return false;
    if (position + size <= other.position)
        return false;
    Physics::Plane intersection;
	std::set_intersection(plane.begin(), plane.end(), other.plane.begin(), other.plane.end(), std::inserter(intersection, intersection.begin()));
	if (intersection.empty())
		return false;
    return true;
}

Location::operator bool() const
{
    return !plane.empty() && size > 0;
}

bool Location::operator==(const Location& other) const
{
	return plane == other.plane && position == other.position && size == other.size;
}
bool Location::operator<(const Location& other) const
{
	if (plane < other.plane)
		return true;
	else if (other.plane < plane)
		return false;
	else if (position < other.position)
		return true;
	else if (position > other.position)
		return false;
	else
		return size < other.size;

}


std::wistream& operator>>(std::wistream& s, Location& v)
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
    v.plane = Physics::Plane::map.at(planeStr);
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
