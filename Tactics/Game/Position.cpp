#include "stdafx.h"
#include "Game/Position.h"
#include "Game/El.h"
#include "Engine/Maths.h"

namespace Game
{

unsigned Position::DistanceEl(const Position& other) const
{
    return static_cast<unsigned>(std::round(std::sqrtf(
        Engine::Sqr(float(HorizontalEl*(other.x - x))) +
        Engine::Sqr(HorizontalEl * (other.y - y)) +
        Engine::Sqr(VerticalEl* (other.z - z))
    ) + 0.0132)); // special bias so DistanceEl(3,1) > DistanceEl(3,0) 
}


}
