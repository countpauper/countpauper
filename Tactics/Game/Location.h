#pragma once

#include <map>
#include <set>
#include "Trajectory.h"

namespace Game
{
    class Direction;
    
    enum class Plane
    {
        None = 0,
        Left = 1 << 8,
        Right = 1 << 9,
        Coronal = Left + Right,
        Front = 1 << 10,
        Back = 1 << 11,
        Sagittal = Front + Back,
        Top = 1 << 12,
        Bottom = 1 << 13,
        Transversal = Top + Bottom,
        Around = Coronal + Sagittal,
        All = Around + Transversal
    };
    std::wistream& operator>>(std::wistream& s, Plane& plane);
    std::ostream& operator<<(std::ostream& s, const Plane& plane);

    Plane ToPlane(Trajectory trajectory);
    Plane Back(Plane from);
    Plane Front(Plane from);
    Plane Right(Plane from);
    Plane Left(Plane from); 
    Plane Transform(Plane from, Plane to);
    Plane Facing(Direction direction, Direction facing);

    class Location
    {
    public:
		Location();
		Location(Plane plane, unsigned pos, unsigned size = 1);
		Location(Trajectory, unsigned pos, unsigned range = 1);
		Location(const Direction& facing, const Direction&target, int height);
		Location(Location attack, Location facing);
        operator bool() const;

        Plane plane;
        unsigned position;
        unsigned size;	// in el
        bool Contains(const Location& other) const;
    private:
    };

    std::wistream& operator>>(std::wistream& s, Location& v);

}    // ::Game
