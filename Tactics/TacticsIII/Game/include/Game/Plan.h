#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Game/Action.h"
#include "Game/Move.h"
#include "Geometry/Position.h"
#include "Geometry/Mesh.h"
#include "UI/Scenery.h"

namespace Game
{
namespace UI
{
    class Avatar;
}
class World;

class Plan : public Engine::Scenery
{
public:
    Plan();
    Plan(const Plan&) = delete;
    Plan& operator=(const Plan&) = delete;
    Plan(Plan&& other);
    Plan& operator=(Plan&& rhs);

    void Render() const override;
    std::string_view Name() const;
    std::string Description() const;

    unsigned AP() const;
    std::string Execute();

    static Plan Move(World& world, Actor& actor, Position destination, unsigned distance=0);
    static Plan Attack(World& world, Actor& actor, Actor& target); 

    template<typename A, typename... Params> 
    requires std::is_base_of_v<Action, A>
    static Plan Act(World& world, Actor& actor, Params&&... params)
    {
        auto action = std::make_unique<A>(world, actor, std::forward<Params>(params)...);
        Plan plan;
        if constexpr(std::tuple_size<typename A::Targets>()>0)
        {
            plan = Move(world, actor, action->GetDestination(), action->GetDistance());
        }
        plan.actions.emplace_back(std::move(action));
        return plan;
    }
private:
    Engine::Mesh mesh;
    // TODO: plans may be a chance tree instead. if the first action is a provoked reaction that might ko the actor, then that's also a possible execution.
    // same for the actions of the actor itself that are chance based
    std::vector<std::unique_ptr<Action>> actions;
    std::string description;
};



}
