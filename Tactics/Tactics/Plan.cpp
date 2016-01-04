#include "stdafx.h"
#include "Plan.h"
#include "Action.h"
#include "Actor.h"

namespace Game
{
    Plan::Plan(const Actor& actor) :
        start(actor)
    {
    }
    void Plan::Render() const
    {
        State state = start;
        for (const auto& node : actions)
        {
            node.action->Render(state);
            state = node.result;
        }
    }

    Plan::Node::Node(std::unique_ptr<Action> action, const State& state) :
        action(std::move(action)),
        result(state)
    {
    }

    Plan::Node::Node(Node&& other) :
        action(std::move(other.action)),
        result(other.result)
    {
    }

    Plan::Node& Plan::Node::operator= (Plan::Node&& other)
    {
        action = std::move(other.action);
        result = other.result;
        return *this;
    }

    void Plan::Add(std::unique_ptr<Action> action, const State& state)
    {
        actions.emplace_back(Node(std::move(action), state));
    }
    void Plan::AddFront(std::unique_ptr<Action> action, const State& state)
    {
        actions.emplace(actions.begin(), Node(std::move(action), state));
    }

    State Plan::Final() const
    {
        if (actions.empty())
            return start;
        else
            return actions.back().result;
    }
    void Plan::Execute(Actor& actor) const
    {
        for (const auto& action : actions)
            if (auto targetedAction = dynamic_cast<TargetedAction*>(action.action.get()))
                targetedAction->React();
        actor.Apply(Final());
    }
}    // ::Game