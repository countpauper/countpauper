#include "UI/Avatar.h"
#include "UI/Application.h"
#include "UI/WindowMessages.h"
#include "Geometry/Matrix.h"
#include "UI/GameMessages.h"
#include "Utility/String.h"

namespace Game
{

Avatar::Avatar(std::string_view name, const  Game::Race& race, Map& map, Engine::Position pos) :
    Scenery(mesh),
    map(map),
    creature(name, race),
    position(pos)
{
    mesh += Engine::Box(Engine::Vector(0.75, 0.75, 1.75));
    mesh *= Engine::Matrix::Translation({-0.375, -0.375, 0.0});
    mesh.SetColor(Engine::RGBA::red);
    mesh.SetName(0);
    Engine::Box selection(Engine::Vector(0.90, 0.90, 0.05));
    selection *= Engine::Matrix::Translation({-0.45, -0.45, 0.0});
    selection.SetName(1);
    selection.SetColor(Engine::RGBA::transparent);
    mesh += selection;

    Engine::Application::Get().bus.Subscribe(*this, {
            Engine::MessageType<Game::Selected>()
        });
}

void Avatar::OnMessage(const Engine::Message& message)
{
    if (auto selection = message.Cast<Selected>())
    {
        if (selection->avatar != this)
        {
            mesh.SetColor(1, Engine::RGBA::transparent);
        }
        else
        {
            mesh.SetColor(1, Engine::RGBA::white);
        }
        Engine::Application::Get().bus.Post(Engine::Redraw());
    }
}

void Avatar::Select(bool on)
{
    if (selected == on)
        return;
    selected = on;
    Engine::Application::Get().bus.Post(Selected(selected ? this: nullptr));
}


void Avatar::Move(Engine::Position destination)
{
    position = destination;
}

Engine::Position Avatar::Position() const
{
    return position;
}

Creature& Avatar::GetCreature()
{
    return creature;
}

Engine::Coordinate Avatar::GetLocation() const
{
    return map.GroundCoord(position);
}

std::string_view Avatar::Name() const
{
    return creature.Name();
}


std::string Avatar::Sheet() const
{
    std::stringstream ss;
    ss << Name() << std::endl;
    ss << Engine::TitleCase(creature.GetRace().Name()).c_str() << " " << creature.Get(Stat::level).Total() << std::endl;
    ss << "HP:" << creature.CounterAvailable(Stat::hp) << " AP:" << creature.CounterAvailable(Stat::ap);
    return ss.str();
}

}
