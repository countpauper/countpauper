#include "Game/Move.h"
#include "Game/Game.h"
#include "AI/Astar.h"
#include "Geometry/Line.h"
#include <GL/gl.h>

namespace Game
{

Move::Move(Avatar& actor, const Game& game, Engine::Position destination) :
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
        Engine::Position neighbourVectors[] {{1,0,0}, {-1,0,0}, {0,1,0},{0,-1,0}};
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
    path = Engine::Astar::Plan<Engine::Position, float>(actor.Position(), destination, cost, neighbours);
}

void Move::Render() const
{
    auto prev = actor.GetLocation();
    glColor3d(1.0, 1.0, 1.0);
    for(auto p: path)
    {
        auto next = map.GroundCoord(p);
        Engine::Line line(prev, next);
        line += Engine::Vector::Z;
        line.Render();
        prev = next;
    }
}

void Move::Execute() const
{
    actor.Move(path.back());
}

unsigned Move::AP() const
{
    auto grids = path.size();
    auto speed = actor.GetCreature().Get(Stat::speed).Total();
    return (grids+(speed-1)) / speed;
}

std::string Move::Description() const
{
    std::stringstream ss;
    ss << "Move " << path.back();
    return ss.str();
}

}
