#pragma once

namespace Game
{

enum class Trajectory {
    // Vector
	None=0,		 // Null
    Forward,     // forward relative to direction, trust
    Backward,	 // backwards relative to direction
    Left,		 // right to left, relative to direction
    Right,		 // left to right, relative to direction
    Down,        // up to down
    Up,          // down to up
};

}    // ::Game
