#pragma once 
#include "UI/Scenery.h"
#include "Game/Actor.h"
#include "Geometry/Mesh.h"

namespace Game
{

class Obstacle : 
    public Engine::Scenery,
    public Actor    
{
public:
    Obstacle();
    // Scenery Interface 
    Engine::Coordinate GetCoordinate() const override;

protected:
    Engine::Mesh mesh;
    Engine::Coordinate coordinate;
};


}