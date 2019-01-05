#pragma once

namespace Game
{

enum class Trajectory {
    Self,        // can  target self 
    Straight,    // forward relative to direction
    Reverse,     // backwards relative to direction
    Forehand,    // right to left, relative to direction
    Backhand,    // left to right, relative to direction
    Down,        // up to down
    Up,          // down to up
    Aim,         // can target body part at will
    Seek,        // can go around cover same as teleport?
    Parabola,    // can go over cover
};

}    // ::Game
