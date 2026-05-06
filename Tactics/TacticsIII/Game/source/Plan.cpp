#include "Game/Plan.h"
#include "Game/Move.h"
#include "Game/Attack.h"
#include "UI/Avatar.h"
#include <numeric>
#include <GL/gl.h>

namespace Game
{

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
        if (ss.tellp())
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

std::string Plan::Execute()
{
    std::stringstream s;
    for(const auto& action: actions)
    {
        auto deltas = action->Execute(s);
        // TODO: in the future the plan will be made on deltas and only the final deltas need to be applied
        for(auto& delta: deltas)
        {
            delta.Apply();
        }
    }
    return s.str();
}

Plan Plan::Move(World& world, Actor& actor, Position destination, unsigned distance)
{
    Plan result;
    result.actions.emplace_back(std::move(std::make_unique<::Game::Move>(world, actor, destination, distance)));
    return result;
}

Plan Plan::Attack(World& world, Actor& actor, UI::Avatar& target)
{
    return Act<Game::Attack>(world, actor, target);
}

}
