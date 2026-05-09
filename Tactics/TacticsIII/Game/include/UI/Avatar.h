#pragma once

#include "Game/Creature.h"
#include "Game/Race.h"
#include "Game/Obstacle.h"
#include "UI/Bus.h"


namespace Game::UI
{
class Counter;

class Avatar :
    public Obstacle,
    public Engine::Passenger
{
public:
    Avatar(const World& world, const Races& races, const class ItemDatabase& items, const json& data);
    Avatar(std::string_view name, const Race& race);
    std::string_view Name() const override;
    std::string Sheet() const;
    void OnMessage(const Engine::Message& message);
    Engine::Quaternion GetOrientation() const override;
    void Select(bool on);
    void Move(const World& world, Position destination) override;

    const Engine::Object& GetAppearance() const override;
    Position GetPosition() const override;
    Size GetSize() const override;
    
    Statted& GetStats() override;
    const Statted& GetStats() const override;
    Counted& GetCounts() override;
    const Counted& GetCounts() const override;
    const class Equipped& GetEquipment() const override;
    class Equipped& GetEquipment() override;
    class Conditions& GetConditions() override;
    const class Conditions& GetConditions() const override;
    json Serialize() const;
private:
    void GenerateMesh();
    void SubscribeBus();
    Creature creature;
    bool selected = false;
};

}
