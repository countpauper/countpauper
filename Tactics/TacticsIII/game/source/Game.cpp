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
    scene.GetCamera().Move(Engine::Coordinate(map.GetSize().x/2, -1, map.GetSize().z));
    scene.GetCamera().Face(map.GroundCoord(Engine::Position(map.GetSize().x / 2, map.GetSize().y / 2, 0)));
    avatars.emplace_back(std::move(std::make_unique<Avatar>("Velg'larn", elf, map, Engine::Position{3, 2})));
    scene.Add(*avatars.back());
    avatars.emplace_back(std::move(std::make_unique<Avatar>("Elg'caress", elf, map, Engine::Position{5, 8})));
    scene.Add(*avatars.back());
}



}
