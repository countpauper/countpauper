#pragma once

#include "Physics/Plane.h"

namespace Game
{

    class Location
    {
    public:
		Location();
		Location(const Physics::Plane& plane, unsigned pos, unsigned size = 1);
        operator bool() const;
		bool operator==(const Location& other) const;
		bool operator<(const Location& other) const;

        Physics::Plane plane;	// normal vectors
        unsigned position;
        unsigned size;	// in el
        bool Contains(const Location& other) const;
    private:
    };

    std::wistream& operator>>(std::wistream& s, Location& v);

}    // ::Game
