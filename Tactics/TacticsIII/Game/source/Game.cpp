#include "Game/Map.h"
#include "UI/Avatar.h"
#include "Game/Game.h"
#include "Game/Equipment.h"
#include "UI/Scene.h"
#include "UI/Application.h"
#include "UI/GameMessages.h"
#include "UI/WindowMessages.h"
#include "UI/Window.h"
#include "UI/Label.h"
#include "UI/HUD.h"

namespace Game
{

Avatars DeserializeAvatars(const HeightMap& map, const Races& races, const ItemDatabase& items, const json& data)
{
    Avatars result;
    for(auto avatarData : data)
    {
        result.emplace_back(std::move(std::make_unique<Avatar>(map, races, items, avatarData)));
    }
    return std::move(result);
}

Game::Game(Engine::Scene& scene, const json& data) :
    scene(scene),
    races(Engine::LoadJson(Engine::get_value_or<std::string_view>(data, "races", "data/races.json"))),
    items(Engine::LoadJson(Engine::get_value_or<std::string_view>(data, "items", "data/items.json"))),
    map(Engine::get_value_or<std::string_view>(data, "map", "data/map20.png")),
    round(Engine::get_value_or<unsigned>(data, "round", 0)),
    turn(Engine::get_value_or<unsigned>(data, "turn", 0))
{
    Creature::definition.Parse(Engine::LoadJson("data/creature.json"));
    avatars = DeserializeAvatars(map, races, items, Engine::get_value_or(data, "avatars", json::array()));

    Engine::Application::Get().bus.Subscribe(*this,
    {
        Engine::MessageType<Selected>(),
        Engine::MessageType<Engine::ClickOn>(),
        Engine::MessageType<Engine::KeyPressed>()
    });
    scene.Add(map);
    scene.Add(plan);
    scene.GetCamera().Move(Engine::Coordinate(map.GetBounds().x.Middle(), -1, map.GetBounds().z.end));

    Focus(Engine::Position(map.GetBounds().x.Middle(), map.GetBounds().y.Middle(), 0));
    for(const auto& avatar: avatars)
    {
        scene.Add(*avatar);
    }
    avatars[turn]->Select(true);
    // const Item& offhand = items.Get("dagger");
    // avatars.at(0)->GetEquipment().Equip(Equipment(offhand));
}

const HeightMap& Game::GetMap() const
{
    return map;
}

bool Game::Obstacle(Engine::Position at, const Actor* except) const
{
    for(const auto& avatar : avatars)
    {
        if (avatar.get() == except)
            continue;
        if (avatar->Position() == at)
            return true;
    }
    // TODO: objects, size &  height (use Bounds)
    return false;
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
    if (auto clickOn = message.Cast<Engine::ClickOn>())
    {
        auto target = dynamic_cast<const Avatar*>(clickOn->object);
        if (clickOn->object == &map)
        {
            auto bounds = map.GetBounds();
            Engine::Position destination(
                    clickOn->sub % bounds.x.Size() + bounds.x.begin,
                    clickOn->sub / bounds.x.Size() + bounds.y.begin,
                    0);
            plan = Plan::Move(Current(), *this, destination);
        }
        else if (target && target!=& Current())
        {
            plan = Plan::Attack(Current(), *this, const_cast<Avatar&>(*target));
        }
        auto lbl = Engine::Window::CurrentWindow()->GetHUD().Find<Engine::Label>("right_lbl");
        lbl->SetText(plan.Description());
        Changed();
    }
    else if (auto selected = message.Cast<Selected>())
    {
        if (selected->avatar)
        {
            Focus(selected->avatar->GetCoordinate());
            auto lbl = Engine::Window::CurrentWindow()->GetHUD().Find<Engine::Label>("left_lbl");
            lbl->SetText(selected->avatar->Sheet());
        }
        else
        {
             Focus(Engine::Position(map.GetBounds().x.Middle(), map.GetBounds().y.Middle(), 0));
        }
        Changed();
    }
    else if (auto key = message.Cast<Engine::KeyPressed>())
    {
        if (key->ascii == 13)
        {
            auto desc = plan.Execute();
            auto log = Engine::Window::CurrentWindow()->GetHUD().Find<Engine::Label>("log_lbl");
            log->AddText(desc);

            plan = Plan();
            if (Current().GetCounts().Available(Stat::ap) == 0)
                Next();
            else
                Current().GetCounts().Reset(Counter::Reset::action);
            Changed();
        }
        else if (key->code == 2)
        {
            Engine::SaveJson(Serialize(), "savegame.json");
        }
    }
}

Avatar& Game::Current() const
{
    return *avatars[turn];
}

void Game::Next()
{
    Current().Select(false);
    turn++;
    if (turn>=avatars.size())
    {
        turn = 0;
        ++round;
    }
    Current().GetCounts().Reset(Counter::Reset::turn);
    Current().Select(true);
}

void Game::Changed()
{
        Engine::Application::Get().bus.Post(Engine::Redraw(&Current()));
}


json Game::Serialize() const
{
    auto avatarArray = json::array();
    for(const auto& avatarPtr : avatars)
    {
        avatarArray.push_back(avatarPtr->Serialize());
    }
    json result =  json::object({
        {"map", map.FileName()},
        {"turn", turn},
        {"round", round},
        {"items", "data/items.json"},
        {"races", "data/races.json"},
        // TODO: stat definitions.json. If not text on load then it's an object with the whole thing
        {"avatars", avatarArray}
    });
    return result;
}

}
