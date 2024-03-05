#pragma once
#include "Game/Map.h"
#include "Game/Race.h"
#include "UI/Avatar.h"
#include "UI/Bus.h"
#include "Game/Plan.h"
#include <memory>

namespace Engine
{
    class Scene;
}

namespace Game
{

class Game : public Engine::Passenger
{
public:
    explicit Game(Engine::Scene& scene);
    const Map& GetMap() const;
    bool Obstacle(Engine::Position at, const Avatar* except=nullptr) const;
private:
    Avatar& Current() const;
    void Focus(Engine::Position pos);
    void Focus(Engine::Coordinate coord);
    void OnMessage(const Engine::Message& message);
    void Next();

    Engine::Scene& scene;
    Race elf;
    Map map;
    Plan plan;
    std::vector<std::unique_ptr<Avatar>> avatars;
    unsigned turn=0;
    unsigned round=0;
};

}
