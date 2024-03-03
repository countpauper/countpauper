#include "Game/Plan.h"
#include "UI/Avatar.h"
#include "Geometry/Line.h"
#include <numeric>
#include <GL/gl.h>

namespace Game
{

Action::Action(Avatar& actor) :
    actor(actor)
{
}

Move::Move(Avatar& actor, const Map& map, Engine::Position destination) :
    Action(actor),
    map(map),
    destination(destination)
{

}

void Move::Render() const
{
    glColor3d(1.0, 1.0, 1.0);
    Engine::Line line(actor.GetLocation(), map.GroundCoord(destination));
    line += Engine::Vector::Z;
    line.Render();
}

void Move::Execute() const
{
    actor.Move(destination);
}

unsigned Move::AP() const
{
    auto grids = actor.Position().ManDistance(destination);
    auto speed = actor.GetCreature().Get(Stat::speed).Total();
    return (grids+(speed-1)) / speed;
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
    return "Plan"; // TODO could be specific plan description
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

Plan Plan::Move(Avatar& actor, const Map& map, Engine::Position destination)
{
    Plan result;
    result.actions.emplace_back(std::move(std::make_unique<Game::Move>(actor, map, destination)));
    return result;
}



}
