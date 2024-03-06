#include "Game/Plan.h"
#include "Game/Game.h"
#include "UI/Avatar.h"
#include "AI/Astar.h"
#include "Geometry/Line.h"
#include "Utility/String.h"
#include <numeric>
#include <GL/gl.h>

namespace Game
{

Action::Action(Avatar& actor) :
    actor(actor)
{
}

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

Plan::Plan() :
    Scenery(mesh)
{
}

Plan::Plan(Plan&& other) :
    Scenery(mesh),
    mesh(std::move(other.mesh))
{
}

Plan& Plan::operator=(Plan&& rhs)
{
    actions.clear();
    std::swap(actions, rhs.actions);
    std::swap(mesh, rhs.mesh);
    return *this;
}

void Plan::Render() const
{
    for(const auto& action: actions)
        action->Render();
}

std::string_view Plan::Name() const
{
    return "Plan";
}

std::string Plan::Description() const
{
    std::stringstream ss;
    for(const auto& act: actions)
    {
        if (ss.tellg())
        {
            ss << "\n";
        }
        ss << act->AP() << "AP: ";
        ss << act->Description();
    }
    return ss.str();
}

unsigned Plan::AP() const
{
    return std::accumulate(actions.begin(), actions.end(), 0, [](unsigned ap, const std::unique_ptr<Action>& a)
    {
        return ap + a->AP();
    });
}

void Plan::Execute()
{
   for(const auto& action: actions)
        action->Execute();
}

Plan Plan::Move(Avatar& actor, const Game& game, Engine::Position destination)
{
    Plan result;
    result.actions.emplace_back(std::move(std::make_unique<::Game::Move>(actor, game, destination)));
    return result;
}



}
