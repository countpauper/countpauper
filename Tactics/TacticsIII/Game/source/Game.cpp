#include "Game/Map.h"
#include "UI/Avatar.h"
#include "Game/Game.h"
#include "Game/UpDown.h"
#include "Game/Equipment.h"
#include "Game/PlanFactory.h"
#include "UI/Scene.h"
#include "UI/Application.h"
#include "UI/GameMessages.h"
#include "UI/WindowMessages.h"
#include "UI/Window.h"
#include "UI/Label.h"
#include "UI/HUD.h"

namespace Game
{

UI::Avatars DeserializeAvatars(const World& world, const Races& races, const ItemDatabase& items, const json& data)
{
    UI::Avatars result;
    for(auto avatarData : data)
    {
        result.emplace_back(std::move(std::make_unique<UI::Avatar>(world, races, items, avatarData)));
    }
    return std::move(result);
}

Game::Game(Engine::Scene& scene, const json& data) :
    scene(scene),
    races(Engine::LoadJson(Engine::get_value_or<std::string_view>(data, "races", "data/races.json"))),
    items(Engine::LoadJson(Engine::get_value_or<std::string_view>(data, "items", "data/items.json"))),
    map(Engine::get_value_or<std::string_view>(data, "map", "data/map20.png")),
    round(Engine::get_value_or<unsigned>(data, "round", 0)),
    turn(Engine::get_value_or<unsigned>(data, "turn", 0)),
    selectedAction(*this)
{
    Creature::definition.Parse(Engine::LoadJson("data/creature.json"));
    avatars = DeserializeAvatars(*this, races, items, Engine::get_value_or(data, "avatars", json::array()));

    Engine::Application::Get().bus.Subscribe(*this,
    {
        Engine::MessageType<UI::Selected>(),
        Engine::MessageType<Engine::KeyPressed>(),
        Engine::MessageType<PlanFactoryIF::Complete>()

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

const MapItf& Game::GetMap() const
{
    return map;
}

bool Game::Obstacle(Position at, const Actor* except) const
{
    for(const auto& avatar : avatars)
    {
        if (avatar.get() == except)
            continue;
        if (avatar->At(at))
            return true;
    }
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
    if (auto complete = message.Cast<PlanFactoryIF::Complete>())
    {
        plan = complete->factory(*this, Current());
        auto lbl = Engine::Window::CurrentWindow()->GetHUD().Find<Engine::Label>("right_lbl");
        lbl->SetText(plan.Description());
        Changed();        
    }
    else if (auto selected = message.Cast<UI::Selected>())
    {
        if (selected->avatar)
        {
            Focus(selected->avatar->GetCoordinate());
            auto lbl = Engine::Window::CurrentWindow()->GetHUD().Find<Engine::Label>("left_lbl");
            lbl->SetText(selected->avatar-> Sheet());
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
        {   // TODO make this a button with a hotkey on the actions bar
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
        {   // TODO make this a button with a hotkey, remove game key subscription 
            Engine::SaveJson(Serialize(), "savegame.json");
        }
    }
}

UI::Avatar& Game::Current() const
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
