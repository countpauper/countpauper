#include "UI/Avatar.h"
#include "UI/Application.h"
#include "UI/WindowMessages.h"
#include "Geometry/Matrix.h"
#include "UI/GameMessages.h"

namespace Game
{

Avatar::Avatar(Map& map, Creature& c, Engine::Position pos) :
    Scenery(mesh),
    map(map),
    creature(c),
    position(pos)
{
    mesh += Engine::Box(Engine::Vector(0.75, 0.75, 1.75));
    mesh *= Engine::Matrix::Translation({-0.5, -0.5, 0.0});
    mesh.SetColor(Engine::RGBA::red);

    Engine::Application::Get().bus.Subscribe(*this, {
            Engine::MessageType<Engine::ClickOn>(),
            Engine::MessageType<Game::Selected>()
        });
}

void Avatar::OnMessage(const Engine::Message& message)
{
    if (auto clickOn = dynamic_cast<const Engine::ClickOn*>(&message))
    {
        if (clickOn->object == this)
        {
            selected = true;
            Engine::Application::Get().bus.Post(Selected(this));
            mesh.SetColor(Engine::RGBA::white);
        }
        else if ((clickOn->object == &map) && (selected))
        {
            auto mapSize = map.GetSize();
            position.x = clickOn->sub % mapSize.x;
            position.y = clickOn->sub / mapSize.x;
        }
    }
    else if (auto selection = dynamic_cast<const Selected*>(&message))
    {
        if (selection->avatar != this)
        {
            selected = false;
            mesh.SetColor(Engine::RGBA::red);
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
