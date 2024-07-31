#include "UI/Avatar.h"
#include "UI/Application.h"
#include "UI/WindowMessages.h"
#include "Geometry/Matrix.h"
#include "UI/GameMessages.h"
#include "Utility/String.h"
#include "Geometry/Angles.h"
#include "Game/Map.h"

namespace Game
{

Avatar::Avatar(std::string_view name, const Race& race) :
    Scenery(mesh),
    creature(name, race)
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


void Avatar::Move(const Map& map, Engine::Position destination)
{
    position = destination;
    coordinate = map.GroundCoord(position);
}


Engine::Position Avatar::Position() const
{
    return position;
}

Statted& Avatar::GetStats()
{
    return creature;
}

const Statted& Avatar::GetStats() const
{
    return creature;
}


Counted& Avatar::GetCounts()
{
    return creature;
}

const Counted& Avatar::GetCounts() const
{
    return creature;
}

double Avatar::HitChance(const Avatar& target) const
{
    auto hitScore = target.GetStats().Get(Stat::dodge) + target.GetStats().Get(Stat::block);    // TODO: front

    return 1.0;
}

Engine::Coordinate Avatar::GetCoordinate() const
{
    return coordinate;
}

Engine::Quaternion Avatar::GetOrientation() const
{
    if (creature.Is<Downed>())
        return Engine::Quaternion(Engine::Vector::Y, Engine::PI/2);
    else    // TODO: add a facing here
        return Engine::Quaternion::Identity;
}

std::string_view Avatar::Name() const
{
    return creature.Name();
}

std::string ConditionList(const Creature& creature)
{
    const auto& conditions = creature.Conditions();
    std::vector<std::string_view> condition_names;
    std::transform(conditions.begin(), conditions.end(), std::back_insert_iterator(condition_names),
    [](const std::reference_wrapper<const Condition>&  c)
    {
        return c.get().Name();
    });
    return Engine::Join(condition_names, ", ");
}

std::string Avatar::Sheet() const
{
    std::stringstream ss;
    ss << Name() << std::endl;
    ss << Engine::TitleCase(creature.GetRace().Name()).c_str() << " " << creature.Get(Stat::level).Total() << std::endl;
    ss << "HP:" << creature.Available(Stat::hp) << " AP:" << creature.Available(Stat::ap) << std::endl;
    ss << ConditionList(creature);
    return ss.str();
}

}
