#include "Game/UpDown.h"
#include "Game/Condition.h"
#include <GL/gl.h>

namespace Game
{

Up::Up(World& world, Actor& actor) :
    ApplySelf(world, actor, *Engine::Singleton<Downed>(), 0)
{
}

void Up::Render() const
{
    auto prev = actor.GetAppearance().GetCoordinate();
    glColor3d(1.0, 1.0, 1.0);
    // TODO draw a billboard arrow Up to normal size
}


unsigned Up::AP() const
{
    return 1;
}



Down::Down(World& world, Actor& actor) :
    ApplySelf(world, actor, *Engine::Singleton<Downed>())
{
}

void Down::Render() const
{
    auto prev = actor.GetAppearance().GetCoordinate();
    glColor3d(1.0, 1.0, 1.0);
    // TODO draw a billboard arrow down from normal size
}


unsigned Down::AP() const
{
    return 1;
}


}
