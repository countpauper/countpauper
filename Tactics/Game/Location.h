#pragma once

#include "Plane.h"

namespace Game
{

    class Location
    {
    public:
		Location();
		Location(const Plane& plane, unsigned pos, unsigned size = 1);
        operator bool() const;
		bool operator==(const Location& other) const;
		bool operator<(const Location& other) const;

		Plane plane;	// normal vectors
        unsigned position;
        unsigned size;	// in el
        bool Contains(const Location& other) const;
    private:
    };

    std::wistream& operator>>(std::wistream& s, Location& v);

}    // ::Game
