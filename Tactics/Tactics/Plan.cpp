#include "stdafx.h"
#include "Plan.h"
#include "Action.h"
#include "Actor.h"
#include "Move.h"
#include "Attack.h"
#include "Skills.h"

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

    Plan::Branch::Branch(const State& result) :
        Node(result),
        previous(nullptr)
    {
    }

    Plan::Branch::Branch(Branch& previous, std::unique_ptr<Action> action, const State& state) :
        Node(std::move(action),state),
        previous(&previous)
    {
    }

    Plan::Node::Node(Node&& other) :
        action(std::move(other.action)),
        result(other.result)
    {
    }

    bool Plan::Branch::Compare(const Branch& other, const Position& target) const
    {
        if (target.ManDistance(result.position) < target.ManDistance(other.result.position))
            return true;
        else if (target.ManDistance(result.position) > target.ManDistance(other.result.position))
            return false;
        else if (result.mp > other.result.mp)
            return true;
        else if (result.mp < other.result.mp)
            return false;
        return
            this < &other;
    }
        
    Plan::BranchCompare::BranchCompare(const Position& target) :
        target(target)
    {
    }

    bool Plan::BranchCompare::operator() (const std::unique_ptr<Branch>& a, const std::unique_ptr<Branch>& b) const
    {
        return a->Compare(*b, target);
    }

    bool Plan::Branch::Reached(const Position& target) const
    {
        return target == result.position;
    }

    Plan::OpenTree::OpenTree(const Position& target) :
        std::set<std::unique_ptr<Branch>, BranchCompare>(BranchCompare(target))
    {
    }

    Plan::ClosedList::ClosedList(const Position& target) :
        std::set<std::unique_ptr<Branch>, BranchCompare>(BranchCompare(target))
    {
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
        if (!node.action)
            return; // TODO: better way to not add root node
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


    void Plan::Approach(const Position& target, const Game& game, std::unique_ptr<Action>&& targetAction)
    {
        std::vector<std::function<Action*(void)>> actions({
            [](){ return new North();  },
            [](){ return new East();  },
            [](){ return new South();  },
            [](){ return new West();  },
        });
        OpenTree open(target);
        ClosedList closed(target);
        State start(actor);
        auto first = std::make_unique<Branch>(start);
        if (first->Reached(target))
            return;
        open.emplace(std::move(first));
        while (!open.empty())
        {
            auto best = open.begin();

            if (targetAction)
            {
                auto finalState = targetAction->Act((*best)->result, game);
                if (finalState.possible)
                {
                    AddFront(**best);
                    for (Branch* previous = (*best)->previous; previous != nullptr; previous = previous->previous)
                    {
                        AddFront(*previous);
                    }
                    Add(std::move(targetAction), finalState);
                }
            }
            else
            {
                if ((*best)->Reached(target))
                {
                    AddFront(**best);
                    for (Branch* previous = (*best)->previous; previous != nullptr; previous = previous->previous)
                    {
                        AddFront(*previous);
                    }
                    return;
                }
            }

            auto bestIt = closed.emplace(std::make_unique<Branch>(*(*best)->previous, std::move((*best)->action), (*best)->result));
            assert(bestIt.second);
            open.erase(best);
            for (const auto& actionFactory : actions)
            {
                std::unique_ptr<Action> action(actionFactory());
                auto newState = action->Act((*bestIt.first)->result, game);
                if (!newState.possible)
                    continue;
                bool alreadyClosed = closed.Contains(newState);
                if (alreadyClosed)    // TODO if new score < closed, still allow? is this possible?
                    continue;
                auto newNode = std::make_unique<Branch>(*(*bestIt.first), std::move(action), newState);
                auto newIt = open.emplace(std::move(newNode));
                assert(newIt.second);
            }
        }
        auto best = closed.begin();
        if ((*best)->action)    // TODO: root node
        {
            AddFront(**best);
            for (auto link = (*best)->previous; link != nullptr; link = link->previous)
            {
                if (link->action)
                    AddFront(*link);
            }
        }
    }

    PathPlan::PathPlan(Actor& actor, const Position& target, const Game& game) :
        Plan(actor),
        target(target)
    {
        Approach(target, game, nullptr);
    }

    AttackPlan::AttackPlan(Actor& actor, Actor& target, const Game& game, const Skill& skill) :
        Plan(actor),
        skill(skill),
        target(target)
    {
        Approach(target.GetPosition(), game, std::unique_ptr<Action>(skill.Action(target)));
    }
}    // 
