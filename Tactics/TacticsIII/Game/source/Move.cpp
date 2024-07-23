#include "Game/Move.h"
#include "Game/Game.h"
#include "AI/Astar.h"
#include "Geometry/Line.h"
#include <GL/gl.h>
#include "Geometry/IntBox.h"

namespace Game
{

std::vector<Engine::Position> Approach(const Map& map, Engine::Position center, unsigned distance)
{
    if (distance==0)
        return {center};

    // Not the most efficient algorithm but with a small map it should be fine
    Engine::IntBox box(center);
    box.Grow(distance);
    std::vector<Engine::Position> result;
    for(auto p : box)
    {
        if (std::round(p.Distance(center) - distance) == 0)
        {
            result.push_back(p);
        }
    }
    return result;
}

Move::Move(Avatar& actor, const Game& game, Engine::Position destination, unsigned distance) :
    Action(actor),
    map(game.GetMap())
{
    auto cost = [](Engine::Position from, Engine::Position to) -> float
    {
        return from.Distance(to);
    };
    auto neighbours = [&game, &actor](Engine::Position at)
    {
        // TODO Return of Direction?
        Engine::Position neighbourVectors[] {{1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}};
        const auto& map = game.GetMap();
        std::vector<Engine::Position> result;
        int jumpHeight = 1+ actor.GetCreature().Get(Stat::jump).Total() / 2;
        for(auto nVec : std::span(neighbourVectors))
        {
            auto to = at + nVec;
            int deltaHeight = map.GroundHeight(to) - map.GroundHeight(at);
            if (deltaHeight > jumpHeight)
                continue;
            if (game.Obstacle(to, &actor))
                continue;
            result.push_back(to);
        }
        return result;
    };
    std::vector destinations = Approach(map, destination, distance);
    path = Engine::Astar::Plan<Engine::Position, float>(actor.Position(), destinations, cost, neighbours);
}

std::vector<Engine::Position>::const_iterator Move::Reachable() const
{
    unsigned grids = actor.GetCreature().Get(Stat::speed).Total() *
        actor.GetCreature().CounterAvailable(Stat::ap);
    if (grids>=path.size())
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
    auto prev = actor.GetCoordinate();
    glColor3d(1.0, 1.0, 1.0);
    auto it = path.begin();
    for(auto p: path)
    {
        if (it++ > Reachable())
            glColor3d(0.9, 0.2, 0.2);
        auto next = map.GroundCoord(p);
        Engine::Line line(prev, next);
        line += Engine::Vector::Z;
        line.Render();
        prev = next;
    }
}

void Move::Execute() const
{
    actor.Move(*Reachable());
    actor.GetCreature().Cost(Stat::ap, AP());
}

unsigned Move::AP() const
{
    auto grids = -1 + path.size();
    auto speed = actor.GetCreature().Get(Stat::speed).Total();
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
