#pragma once
#include "Game/Actor.h"
#include "Game/Creature.h"
#include "Game/Race.h"
#include "Geometry/Position.h"
#include "Geometry/Mesh.h"
#include "UI/Scenery.h"
#include "UI/Bus.h"

namespace Game
{
class Counter;

class Avatar :
    public Engine::Scenery,
    public Actor,
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
    void Move(const class HeightMap& map, Engine::Position destination) override;
    void Equip(const Equipment& equipment) override;

    const Engine::Scenery& GetAppearance() const override;
    Engine::Position Position() const override;
    Statted& GetStats() override;
    const Statted& GetStats() const override;
    Counted& GetCounts() override;
    const Counted& GetCounts() const override;
private:
    Engine::Mesh mesh;
    Engine::Position position;
    Engine::Coordinate coordinate;
    Creature creature;
    bool selected = false;
};

}
