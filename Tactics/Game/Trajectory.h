#pragma once

namespace Game
{

enum class Trajectory {
    // Vector
    Straight,    // forward relative to direction
    Reverse,     // backwards relative to direction
    Forehand,    // right to left, relative to direction
    Backhand,    // left to right, relative to direction
    Down,        // up to down
    Up,          // down to up
};

}    // ::Game
