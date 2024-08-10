#pragma once
#include "Game/Map.h"
#include "UI/Avatar.h"
#include "UI/Bus.h"
#include "Game/Plan.h"
#include "Game/World.h"
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
    explicit Game(Engine::Scene& scene);
    const HeightMap& GetMap() const override;
    bool Obstacle(Engine::Position at, const Actor* except=nullptr) const override;
private:
    Avatar& Current() const;
    void Focus(Engine::Position pos);
    void Focus(Engine::Coordinate coord);
    void OnMessage(const Engine::Message& message);
    void Next();
    void Changed();
    Engine::Scene& scene;
    Race elf;
    Race orc;
    Map map;
    Plan plan;
    std::vector<std::unique_ptr<Avatar>> avatars;
    unsigned turn=0;
    unsigned round=0;
};

}
