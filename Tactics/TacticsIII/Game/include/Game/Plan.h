#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Game/Action.h"
#include "Geometry/Position.h"
#include "Geometry/Mesh.h"
#include "UI/Scenery.h"

namespace Game
{
class Avatar;
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

    // TODO: helpers to create an attack plan, flee (move) plan, spell/technique plan, dodge, ready and so on plan for UI and AI level
    static Plan Move(World& world, Avatar& actor, Engine::Position destination);
    static Plan Attack(World& world, Avatar& actor, Avatar& target);
private:
    Engine::Mesh mesh;
    // TODO: plans may be a chance tree instead. if the first action is a provoked reaction that might ko the actor, then that's also a possible execution.
    // same for the actions of the actor itself that are chance based
    std::vector<std::unique_ptr<Action>> actions;
    std::string description;
};



}
