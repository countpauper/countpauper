#pragma once
#include "UI/Scenery.h"
#include "Game/Creature.h"
#include "Game/Map.h"
#include "Geometry/Position.h"
#include "UI/Bus.h"

namespace Game
{

class Avatar :
        public Engine::Scenery,
        public Engine::Passenger
{
public:
    explicit Avatar(Map& map, Creature& c, Engine::Position pos);
    std::string_view Name() const override;
    void OnMessage(const Engine::Message& message);
    Engine::Coordinate GetLocation() const override;
private:
    Engine::Mesh mesh;
    Map& map;
    Engine::Position position;
    Creature& creature;
    bool selected = false;
};

}
