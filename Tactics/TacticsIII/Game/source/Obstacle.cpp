#include "Game/Obstacle.h"

namespace Game 
{

Obstacle::Obstacle() :
    Scenery(mesh)
{
}

Engine::Coordinate Obstacle::GetCoordinate() const
{
    return coordinate;
}


};

