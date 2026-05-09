#pragma once
#include "Game/Map.h"
#include "UI/Avatar.h"
#include "UI/Bus.h"
#include "UI/ActionSelection.h"
#include "Game/Plan.h"
#include "Game/World.h"
#include "Game/Race.h"
#include "Game/ItemDatabase.h"
#include <vector>
#include <memory>

namespace Engine
{
    class Scene;
}

namespace Game
{

class Game :
    public Engine::Passenger,
    public World
{
public:
    explicit Game(Engine::Scene& scene, const json& data=json());
    const MapItf& GetMap() const override;
    const Actor* ObstacleAt(const Engine::IntBox& bounds, const Actor* except=nullptr) const override;
    json Serialize() const;

    using Obstacles = std::vector<std::unique_ptr<Obstacle>>;

private:
    UI::Avatar& Current() const;
    void Focus(Engine::Position pos);
    void Focus(Engine::Coordinate coord);
    void OnMessage(const Engine::Message& message);
    void Next();
    void Changed();
    Engine::Scene& scene;
    Races races;
    ItemDatabase items;
    Map map;
    Obstacles obstacles;
    std::vector<UI::Avatar*> avatars;
    Plan plan;
    UI::ActionSelection selectedAction;
    unsigned round=0;
    unsigned turn=0;
};

}
