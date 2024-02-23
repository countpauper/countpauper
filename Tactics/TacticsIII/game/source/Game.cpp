#include "File/Image.h"
#include "Game/Map.h"
#include "UI/Avatar.h"
#include "Game/Game.h"
#include "UI/Scene.h"

namespace Game
{

Game::Game(Engine::Scene& scene) :
    elf("elf"),
    map(Engine::Image("data/map20.png"))
{
    scene.Add(map);
    avatars.emplace_back(std::move(std::make_unique<Avatar>("Velg'larn", elf, map, Engine::Position{3, 2})));
    scene.Add(*avatars.back());
    avatars.emplace_back(std::move(std::make_unique<Avatar>("Elg'caress", elf, map, Engine::Position{5, 8})));
    scene.Add(*avatars.back());
}



}
