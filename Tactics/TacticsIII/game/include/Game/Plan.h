#pragma once
#include <vector>
#include <memory>
#include "Geometry/Position.h"
#include "Geometry/Mesh.h"
#include "UI/Scenery.h"

namespace Game
{
class Avatar;
class Map;

class Action
{
public:
    Action(Avatar& actor);
    virtual void Render() const = 0;
    virtual unsigned AP() const = 0;
    virtual void Execute() const = 0;
protected:
    Avatar& actor;
};

class Move : public Action
{
public:
    Move(Avatar& actor, const Map& map, Engine::Position destination);
    void Render() const override;
    void Execute() const override;
    unsigned AP() const override;
private:
    const Map& map;
    Engine::Position destination;
};


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
    unsigned AP() const;
    void Execute();

    // TODO: helpers to create an attack plan, flee (move) plan, spell/technique plan, dodge, ready and so on plan for UI and AI level
    static Plan Move(Avatar& actor, const Map& map, Engine::Position destination);
private:
    Engine::Mesh mesh;
    // TODO: plans may be a chance tree instead. if the first action is a provoked reaction that might ko the actor, then that's also a possible execution.
    // same for the actions of the actor itself that are chance based
    std::vector<std::unique_ptr<Action>> actions;
};



}
