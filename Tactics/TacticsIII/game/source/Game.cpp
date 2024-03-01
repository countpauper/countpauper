#include "File/Image.h"
#include "Game/Map.h"
#include "UI/Avatar.h"
#include "Game/Game.h"
#include "UI/Scene.h"
#include "UI/Application.h"
#include "UI/GameMessages.h"
#include "UI/WindowMessages.h"
#include <fstream>

namespace Game
{

Game::Game(Engine::Scene& scene) :
    scene(scene),
    elf("elf"),
    map(Engine::Image("data/map20.png"))
{
    std::ifstream defFile("data/creature.json");
    Creature::definition.Load(defFile);
    Engine::Application::Get().bus.Subscribe(*this,
    {
        Engine::MessageType<Selected>(),
        Engine::MessageType<Engine::KeyPressed>()
    });
    scene.Add(map);
    scene.GetCamera().Move(Engine::Coordinate(map.GetSize().x/2, -1, map.GetSize().z));

    Focus(Engine::Position(map.GetSize().x / 2, map.GetSize().y / 2, 0));
    avatars.emplace_back(std::move(std::make_unique<Avatar>("Velg'larn", elf, map, Engine::Position{3, 2})));
    scene.Add(*avatars.back());
    avatars.emplace_back(std::move(std::make_unique<Avatar>("Elg'caress", elf, map, Engine::Position{5, 8})));
    scene.Add(*avatars.back());
    avatars[turn]->Select(true);
}

void Game::Focus(Engine::Position pos)
{
    Focus(map.GroundCoord(pos));
}

void Game::Focus(Engine::Coordinate coord)
{
    if (auto trackCamera = dynamic_cast<Engine::TrackingCamera*>(&scene.GetCamera()))
    {
        trackCamera->Track(coord);
    }
    else
    {
        scene.GetCamera().Face(coord);
    }
}

void Game::OnMessage(const Engine::Message& message)
{
    if (auto selected = message.Cast<Selected>())
    {
        if (selected->avatar)
        {
            Focus(selected->avatar->GetLocation());
        }
        else
        {
             Focus(Engine::Position(map.GetSize().x / 2, map.GetSize().y / 2, 0));
        }
    } else if (auto key = message.Cast<Engine::KeyPressed>())
    {
        if (key->ascii == 13)
        {
            Next();
        }
    }
}

void Game::Next()
{
    avatars[turn]->Select(false);
    turn++;
    if (turn>=avatars.size())
    {
        turn = 0;
        ++round;
    }
    avatars[turn]->Select(true);
}



}
