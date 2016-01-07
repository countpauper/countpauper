#include "stdafx.h"
#include "Plan.h"
#include "Action.h"
#include "Actor.h"
#include "Move.h"

namespace Game
{
    Plan::Plan(Actor& actor) :
        actor(actor)
    {
    }

    void Plan::Render() const
    {
        State state(actor);
        for (const auto& node : actions)
        {
            node.action->Render(state);
            state = node.result;
        }
    }

    Plan::Node::Node(const State& result) :
        result(result)
    {
    }

    Plan::Node::Node(std::unique_ptr<Action> action, const State& result) :
        action(std::move(action)),
        result(result)
    {
    }

    Plan::Branch::Branch(const State& result, const Position& target) :
        Node(result),
        previous(nullptr),
        target(target)  // TODO not in every link
    {
    }

    Plan::Branch::Branch(Branch& previous, std::unique_ptr<Action> action, const State& state, const Position& target) :
        Node(std::move(action),state),
        previous(&previous),
        target(target)
    {
    }

    Plan::Node::Node(Node&& other) :
        action(std::move(other.action)),
        result(other.result)
    {
    }

    bool Plan::Branch::operator>(const Plan::Branch& other) const
    {
        if (target.Distance(result.position) < target.Distance(other.result.position))
            return true;
        else if (result.mp > other.result.mp)
            return true;
        return false;
    }
        
    bool Plan::BranchCompare::operator() (const std::unique_ptr<Branch>& a, const std::unique_ptr<Branch>& b) const
    {
        if (*a > *b)
        {
            if (*b > *a)
                return a.get() < b.get();
            else
                return true;
        }
        else
        {
            if (*b > *a)
                return false;
            else
                return a.get() < b.get();
        }
    }


    bool Plan::Branch::Reached() const
    {
        return target == result.position;
    }

    bool Plan::ClosedList::Contains(const State& state) const
    {
        for (const auto& node : *this)
            if (node->result.position == state.position)
                return true;
        return false;
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
    void Plan::AddFront(Node& node)
    {
        actions.emplace(actions.begin(), Node(std::move(node.action), node.result));
    }

    State Plan::Final() const
    {
        if (actions.empty())
            return State(actor);
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


    void Plan::Approach(const Position& target, const Game& game)
    {
        std::vector<std::function<Action*(void)>> actions({
            [](){ return new North();  },
            [](){ return new East();  },
            [](){ return new South();  },
            [](){ return new West();  },
        });
        OpenTree open;
        ClosedList closed;
        State start(actor);
        auto first = std::make_unique<Branch>(start, target);
        if (first->Reached())
            return;
        open.emplace(std::move(first));
        while (!open.empty())
        {
            auto best = open.begin();

            closed.emplace(std::make_unique<Branch>(*(*best)->previous, std::move((*best)->action), (*best)->result, target));
            open.erase(best);
            auto bestIt = closed.rbegin();    // TODO sorted
            for (const auto& actionFactory : actions)
            {
                std::unique_ptr<Action> action(actionFactory());
                auto newState = action->Act((*bestIt)->result, game);
                if (!newState.possible)
                    continue;
                bool alreadyClosed = closed.Contains(newState);
                if (alreadyClosed)    // TODO if new score < closed, still allow? is this possible?
                    continue;
                auto newNode = std::make_unique<Branch>(*(*bestIt)->previous, std::move(action), newState, target);
                if (newNode->Reached())
                {
                    AddFront(*newNode);
                    for (Branch* previous = newNode->previous; previous != nullptr; previous = previous->previous)
                    {
                        if (previous->action)    // TODO: more gracefull detection of root node
                            AddFront(*previous);
                    }
                    return;
                }
                else
                {
                    open.emplace(std::move(newNode));
                }
            }
        }
        auto best = closed.begin();
        for (auto link = (*best)->previous; link != nullptr; link = link->previous)
        {
            if (link->action)
                AddFront(*link);
        }
    }

    PathPlan::PathPlan(Actor& actor, const Position& target, const Game& game) :
        Plan(actor),
        target(target)
    {
        Approach(target, game);
    }
}    // ::Game