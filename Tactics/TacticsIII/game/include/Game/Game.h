#pragma once
#include "Game/Map.h"
#include "Game/Race.h"
#include "UI/Avatar.h"
#include <memory>

namespace Engine
{
    class Scene;
}

namespace Game
{

class Game
{
public:
    explicit Game(Engine::Scene& scene);
private:
    ::Game::Race elf;
    Map map;
    std::vector<std::unique_ptr<Avatar>> avatars;
};

}
