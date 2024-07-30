#pragma once
#include "UI/Scenery.h"
#include "Game/Creature.h"
#include "Game/Race.h"
#include "Geometry/Position.h"
#include "Geometry/Mesh.h"
#include "UI/Bus.h"

namespace Game
{
class Counter;

class Avatar :
        public Engine::Scenery,
        public Engine::Passenger
{
public:
    Avatar(std::string_view name, const Race& race);
    std::string_view Name() const override;
    std::string Sheet() const;
    void OnMessage(const Engine::Message& message);
    Engine::Coordinate GetCoordinate() const override;
    Engine::Quaternion GetOrientation() const override;
    void Select(bool on);
    void Move(const class Map& map, Engine::Position destination);
    Engine::Position Position() const;
    Creature& GetCreature();
    const Creature& GetCreature() const;
    double HitChance(const Avatar& target) const;
private:
    Engine::Mesh mesh;
    Engine::Position position;
    Engine::Coordinate coordinate;
    Creature creature;
    bool selected = false;
};

}
