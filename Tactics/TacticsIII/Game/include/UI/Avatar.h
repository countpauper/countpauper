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
    Avatar(const World& world, const Races& races, const class ItemDatabase& items, const json& data);
    Avatar(std::string_view name, const Race& race);
    std::string_view Name() const override;
    std::string Sheet() const;
    void OnMessage(const Engine::Message& message);
    Engine::Coordinate GetCoordinate() const override;
    Engine::Quaternion GetOrientation() const override;
    void Select(bool on);
    void Move(const World& world, Engine::Position destination) override;

    const Engine::Object& GetAppearance() const override;
    Engine::Position Position() const override;
    Engine::Size Size() const override;
    Statted& GetStats() override;
    const Statted& GetStats() const override;
    Counted& GetCounts() override;
    const Counted& GetCounts() const override;
    const class Equipped& GetEquipment() const override;
    class Equipped& GetEquipment() override;

    json Serialize() const;
private:
    void GenerateMesh();
    void SubscribeBus();

    Engine::Mesh mesh;
    Engine::Coordinate coordinate;
    Creature creature;
    bool selected = false;
};

using Avatars = std::vector<std::unique_ptr<Avatar>>;

}
