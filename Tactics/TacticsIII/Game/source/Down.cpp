#include "Game/Down.h"
#include "Game/Condition.h"
#include "Game/Requirement.h"
#include <GL/gl.h>

namespace Game
{

Down::Down(World& world, Actor& actor) :
    Action(world, actor)
{
}

void Down::Render() const
{
    auto prev = actor.GetAppearance().GetCoordinate();
    glColor3d(1.0, 1.0, 1.0);
    // TODO draw a billboard arrow down from normal size
}


Requirements Down::CanDo() const
{
    return Requirements {
        StatRequirement(Stat::ap, actor.GetCounts().Available(Stat::ap), Comparator::greater_equal, AP()),
        ConditionRequirement(actor.GetConditions().Is<Downed>(), *Engine::Singleton<Downed>(), false)
    };
}

std::vector<Delta> Down::Execute(std::ostream& log) const
{
    auto can = CanDo();
    if (!can)
    {
        log << actor.GetAppearance().Name() << " stays up ";
        return {};
    }
    Delta dActor(actor);
    dActor.GetConditions().Apply<Downed>();
    dActor.GetCounts().Cost(Stat::ap, AP(), true);
    log << actor.GetAppearance().Name() << " goes down";
    return { std::move(dActor) };
}

unsigned Down::AP() const
{
    return 1;
}

std::string Down::Description() const
{
    std::stringstream ss;
    ss << "Go down";
    return ss.str();
}

Position Down::GetDestination() const
{
    return actor.GetPosition();
}

unsigned Down::GetDistance() const
{
    return 0;
}

}
