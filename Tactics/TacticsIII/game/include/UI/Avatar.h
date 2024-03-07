#pragma once
#include "UI/Scenery.h"
#include "Game/Creature.h"
#include "Game/Map.h"
#include "Game/Race.h"
#include "Geometry/Position.h"
#include "UI/Bus.h"
namespace Game
{
class Counter;

class Avatar :
        public Engine::Scenery,
        public Engine::Passenger
{
public:
    Avatar(std::string_view name, const Race& race, Map& map, Engine::Position pos);
    std::string_view Name() const override;
    std::string Sheet() const;
    void OnMessage(const Engine::Message& message);
    Engine::Coordinate GetLocation() const override;
    void Select(bool on);
    void Move(Engine::Position destination);
    Engine::Position Position() const;
    Creature& GetCreature();
private:
    Engine::Mesh mesh;
    Map& map;
    Engine::Position position;
    Creature creature;
    bool selected = false;
};

}
