#include "Game/Move.h"
#include "Game/World.h"
#include "Game/Stat.h"
#include "Game/Actor.h"
#include "Game/HeightMap.h"
#include "Game/Orientation.h"
#include "AI/Astar.h"
#include "Geometry/Line.h"
#include <GL/gl.h>
#include "Geometry/IntBox.h"
#include "UI/Logging.h"

namespace Game
{

std::vector<Engine::Position> Approach(const World& world,  Actor& actor, Engine::Position center, unsigned distance)
{
    // Not the most efficient algorithm but with a small map it should be fine
    Engine::IntBox box(center);
    box.Grow(distance);
    std::vector<Engine::Position> result;
    for(auto p : box)
    {
        if ((std::round(p.Distance(center) - distance) == 0) &&
            (!world.Obstacle(p, &actor)))
        {
            result.push_back(p);
        }
    }
    return result;
}

Move::Move(World& world, Actor& actor, Engine::Position destination, unsigned distance) :
    Action(world, actor)
{
    auto cost = [](Engine::Position from, Engine::Position to) -> float
    {
        return from.Distance(to);
    };
    const auto& map = world.GetMap();
    auto neighbours = [&world, &actor](Engine::Position at)
    {
        const auto& map = world.GetMap();
        std::vector<Engine::Position> result;
        float jump = 1.0f + actor.GetStats().Get(Stat::jump).Total() / 2.0f;
        for(auto ori : Orientation::all)
        {
            auto to = at + ori.Vector();
            if (!map.GetBounds().Contains(to))
                continue;
            Engine::Position headroom = to + Engine::Position(0, 0, std::ceil(jump));
            float deltaHeight = map.GroundHeight(headroom) - map.GroundHeight(at);
            if (std::abs(deltaHeight) > jump)
                continue;
            // TODO check size its under ceiling
            if (world.Obstacle(to, &actor))
                continue;
            result.push_back(to);
        }
        return result;
    };
    std::vector destinations = Approach(world, actor, destination, distance);
    path = Engine::Astar::Plan<Engine::Position, float>(actor.GetPosition().p, destinations, cost, neighbours);
}

std::vector<Engine::Position>::const_iterator Move::Reachable() const
{
    unsigned grids = actor.GetStats().Get(Stat::speed).Total() *
        actor.GetCounts().Available(Stat::ap);
    if (path.empty())
    {
        return path.begin();
    }
    else if (grids>=path.size())
    {
        return path.end() - 1;
    }
    else
    {
        return path.begin()+grids;  // assuming all are normal speed;
    }
}

void Move::Render() const
{
    auto prev = actor.GetAppearance().GetCoordinate();
    glColor3d(1.0, 1.0, 1.0);
    auto it = path.begin();
    for(auto p: path)
    {
        if (it++ > Reachable())
            glColor3d(0.9, 0.2, 0.2);
        auto next = world.GetMap().GroundCoord(p);
        Engine::Line line(prev, next);
        line += Engine::Vector::Z;
        line.Render();
        prev = next;
    }
}


Requirements Move::CanDo() const
{
    Requirements req{
        StatRequirement(Stat::ap, actor.GetCounts().Available(Stat::ap), Comparator::greater_equal, AP()),
        StatRequirement(Stat::speed, actor.GetStats().Get(Stat::speed), Comparator::not_equal, 0)
    };

    if (!req) {
        return req;
    }
    req.push_back(PathRequirement(!path.empty()));
    return req;
}

std::vector<Delta> Move::Execute(std::ostream& log) const
{
    auto can = CanDo();
    if (!can)
    {
        log << actor.GetAppearance().Name() << " can't move, because " << can.Failed().Description() << std::endl;
        return {};
    }
    auto ap = AP();
    Delta dActor(actor);
    if (ap)
    {
        dActor.Move(world, *Reachable());
        dActor.GetCounts().Cost(Stat::ap, ap, true);
        log << actor.GetAppearance().Name() << " moves to " << *Reachable() << std::endl;
    }
    else
    {
        log << actor.GetAppearance().Name() << " stays ";
    }
    return { dActor };
}

unsigned Move::AP() const
{
    auto grids = -1 + path.size();
    auto speed = actor.GetStats().Get(Stat::speed).Total();
    if (speed == 0)
        return 0;   // it takes 0 AP to not move (no CanDo)
    return (grids+(speed-1)) / speed;
}

std::string Move::Description() const
{
    if (path.empty()) {
        return "Don't move";
    }
    std::stringstream ss;
    ss << "Move " << path.back();
    return ss.str();
}

}
