#pragma once
#include "Game/Map.h"
#include "Game/Race.h"
#include "UI/Avatar.h"
#include "UI/Bus.h"
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
private:
    void Focus(Engine::Position pos);
    void Focus(Engine::Coordinate coord);
    void OnMessage(const Engine::Message& message);
    void Next();

    Engine::Scene& scene;
    ::Game::Race elf;
    Map map;
    std::vector<std::unique_ptr<Avatar>> avatars;
    unsigned turn=0;
    unsigned round=0;
};

}
