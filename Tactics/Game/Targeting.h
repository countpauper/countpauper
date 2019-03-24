#pragma once

namespace Game
{

enum class Targeting
{
    // Targeting
    Self,        // can target self, trajectory none
	Face,		 // automatically turn towards target position
    Intercept,   // attempty to counter trajectory of attacker, from used limb
    Swing,       // Adjust -1 to +1 in height from used limb
    Center,      // center mass: gaussian height
    Aim,         // can target body part at will
    Seek,        // can go around cover same as teleport?
    Parabola,    // can go over cover of +1
    Vertical,    // Straight from the heavens or hell
};

}    // ::Game
