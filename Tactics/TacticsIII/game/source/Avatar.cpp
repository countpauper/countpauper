#include "UI/Avatar.h"
#include "UI/Application.h"
#include "UI/WindowMessages.h"
#include "Geometry/Matrix.h"

namespace Game
{

Avatar::Avatar(Map& map, Creature& c) :
    Scenery(mesh),
    map(map),
    creature(c),
    position(2,3,0)
{
    mesh += Engine::Box(Engine::Vector(0.75, 0.75, 1.75));
    mesh *= Engine::Matrix::Translation({-0.5, -0.5, 0.0});
    mesh.SetColor(Engine::RGBA::red);

    Engine::Application::Get().bus.Subscribe(*this, {Engine::MessageType<Engine::ClickOn>()});
}

void Avatar::OnMessage(const Engine::Message& message)
{
    if (auto clickOn = dynamic_cast<const Engine::ClickOn*>(&message))
    {
        if (&clickOn->object == this)
        {
            selected = true;
            mesh.SetColor(Engine::RGBA::white);
        }
        else if ((&clickOn->object == &map) && (selected))
        {
            auto mapSize = map.GetSize();
            position.x = clickOn->sub % mapSize.x;
            position.y = clickOn->sub / mapSize.x;
        }
    }
}

Engine::Coordinate Avatar::GetLocation() const
{
    return map.GroundCoord(position);
}

std::string_view Avatar::Name() const
{
    return creature.Name();
}

}
