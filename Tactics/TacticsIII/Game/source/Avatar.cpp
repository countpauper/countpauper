#include "UI/Avatar.h"
#include "UI/Application.h"
#include "UI/WindowMessages.h"
#include "Geometry/Matrix.h"
#include "UI/GameMessages.h"
#include "Utility/String.h"
#include "File/Json.h"
#include "Geometry/Angles.h"
#include "Game/Map.h"

namespace Game
{

Avatar::Avatar(const HeightMap& map,  const Races& races, const ItemDatabase& items, const json& data) :
    Scenery(mesh),
    creature(races.Get(Engine::must_get<std::string_view>(data, "race")), items, data)
{
    if (auto posData = Engine::try_get<json>(data, "position"))
    {
        Move(map, Engine::Position((*posData)[0], (*posData)[1], (*posData)[2]));
    }
    GenerateMesh();
    SubscribeBus();
}

Avatar::Avatar(std::string_view name, const Race& race) :
    Scenery(mesh),
    creature(name, race)
{
    GenerateMesh();
    SubscribeBus();
}

void Avatar::GenerateMesh()
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
}

void Avatar::SubscribeBus()
{
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


void Avatar::Move(const HeightMap& map, Engine::Position destination)
{
    coordinate = map.GroundCoord(creature.SetPosition(destination));
}


const Engine::Object& Avatar::GetAppearance() const
{
    return *this;
}

Engine::Position Avatar::Position() const
{
    return creature.GetPosition();
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


const class Equipped& Avatar::GetEquipment() const
{
    return creature;
}

class Equipped& Avatar::GetEquipment()
{
    return creature;
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


std::string Avatar::Sheet() const
{
    std::stringstream ss;
    ss << Name() << std::endl;
    ss << Engine::TitleCase(creature.GetRace().Name()).c_str() << " " << creature.Get(Stat::level).Total() << std::endl;
    ss << "HP:" << creature.Available(Stat::hp) << " AP:" << creature.Available(Stat::ap) << std::endl;
    ss << creature.Conditions::Description();
    return ss.str();
}

json Avatar::Serialize() const
{
    json result =  creature.Serialize();
    return result;
}


}
