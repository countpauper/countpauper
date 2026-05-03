#include "Game/Move.h"
#include "Game/World.h"
#include "Game/Stat.h"
#include "Game/Actor.h"
#include "Game/MapItf.h"
#include "Game/Slice.h"
#include "Game/Orientation.h"
#include "AI/Astar.h"
#include "Geometry/Line.h"
#include <GL/gl.h>
#include "Geometry/IntBox.h"
#include "UI/Logging.h"
#include "Utility/Assert.h"

namespace Game
{

std::set<Position> Approach(const World& world,  Actor& actor, Position center, unsigned distance)
{
    // Not the most efficient algorithm but with a small map it should be fine
    Engine::IntBox box(round(center));
    box.Grow(distance);
    std::set<Position> result;
    for(auto p : box)
    {
        Position gp(p);
        if ((std::round(gp.Distance(center) - distance) == 0) &&
            (!world.Obstacle(gp, &actor)))
        {
            result.insert(Position(gp.X(), gp.Y(), world.GetMap().GroundHeight(gp)));
        }
    }
    return result;
}

Engine::Range<ZType> FindMoveOpening(const MapItf& map, Position at, Engine::Range<ZType> heightDifference, ZType space)
{
    const auto& slice = map.SliceAt(at.X(), at.Y());
    auto [it, offset] = slice.Find(at.Z());
    if (it->IsSolid())
    {
        auto delta = it->amount - offset;  
        if (delta > heightDifference.end)
            return Engine::Range<ZType>::empty();
        auto next = it+1;
        if (next == slice.end())
            return Engine::Range<ZType>::empty();
        assert(!next->IsSolid());   // not handled yet, iterate to layers it can pass 
        if (next->amount < space)   
            return Engine::Range<ZType>::empty();   // TODO should add subsequent that can pass for full height 
        return { at.Z() + delta, at.Z() + delta + next->amount };
    }
    else 
    {   
        if (it == slice.begin())
            return Engine::Range<ZType>::empty();
        assert((it-1)->IsSolid());  // subsequent downwards layers that cant support weight not yet handles
        if (offset > -heightDifference.begin)
            return Engine::Range<ZType>::empty();
        if (it->amount < space)
            return Engine::Range<ZType>::empty();
        return { at.Z() - offset, at.Z() - offset + it->amount };    
    }
}

Move::Move(World& world, Actor& actor, Position destination, unsigned distance) :
    Action(world, actor),
    destination(destination),
    distance(distance)
{
    auto cost = [](Position from, Position to) -> float
    {
        return from.Distance(to);
    };
    const auto& map = world.GetMap();
    auto neighbours = [&world, &actor](Position at)
    {
        const auto& map = world.GetMap();
        std::vector<Position> result;
        ZType jumpHeight = ZType(1) + ZType(actor.GetStats().Get(Stat::jump).Total()) / 2;
        ZType actorHeight = actor.GetSize().Z();
        Position jump(0, 0, jumpHeight);

        for(auto ori : Orientations::horizontal)    // NB: Horizontal only because this is specific for walking 
        {
            auto to = at + Position(ori.GetVector());
            if (!map.GetBounds().Contains(round(to)))
                continue;
            auto ground = FindMoveOpening(map, to, {-jumpHeight,jumpHeight}, actorHeight);
            if (ground.Size() < actorHeight || ground.begin == ZType(0))
                continue;   // no ground to stand on
            to.z = ground.begin; 
            // TODO check size its under ceiling
            if (world.Obstacle(to, &actor))
                continue;
            result.push_back(to);
        }
        return result;
    };
    std::set<Position> destinations = Approach(world, actor, destination, distance);
    path = Engine::Astar::Plan<Position, float>(actor.GetPosition(), destinations, cost, neighbours, map.GetBounds().Volume());
}

std::vector<Position>::const_iterator Move::Reachable() const
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
        auto next = p.Coord();
        Engine::Line line(prev, next);
        line += Engine::Vector::ZAxis;
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
        dActor.Move(world, Position(*Reachable()));
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
    std::stringstream ss;
    if (distance==0)
        ss << "Move to " << destination;
    else 
        ss << "Move within " << distance << " of " << destination;
    return ss.str();
}

bool Move::TargetDestination() const 
{
    return true;
}

Position Move::GetDestination() const
{
    return destination;
}

unsigned Move::GetDistance() const
{
    return distance;
}

}
