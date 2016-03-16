#include "stdafx.h"
#include <iostream>
#include "Plan.h"
#include "Action.h"
#include "Actor.h"
#include "Move.h"
#include "Attack.h"
#include "Skills.h"
#include "Game.h"

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
            state = node.ExpectedState(actor);
        }
    }

    Plan::Node::Node(const GameChances& state) :
        result(state)
    {
    }

    Plan::Node::Node(std::unique_ptr<Action> action, const GameChances& outcomes) :
        action(std::move(action)),
        result(outcomes)
    {
    }

    const State& Plan::Node::ExpectedState(const Actor& actor) const
    {
        return result.front().Get(actor);
    }

    Plan::Branch::Branch(const GameChances& result) :
        Node(result),
        previous(nullptr)
    {
    }

    Plan::Branch::Branch(Branch& previous, std::unique_ptr<Action> action, const GameChances& outcomes) :
        Node(std::move(action),outcomes),
        previous(&previous)
    {
    }

    Plan::Node::Node(Node&& other) :
        action(std::move(other.action)),
        result(other.result)
    {
    }

    bool Plan::Branch::Compare(const Actor& actor,const Branch& other, const Position& target) const
    {
        if (target.ManDistance(ExpectedState(actor).position) < target.ManDistance(other.ExpectedState(actor).position))
            return true;
        else if (target.ManDistance(ExpectedState(actor).position) > target.ManDistance(other.ExpectedState(actor).position))
            return false;
        else if (ExpectedState(actor).mp > other.ExpectedState(actor).mp)
            return true;
        else if (ExpectedState(actor).mp < other.ExpectedState(actor).mp)
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

    bool Plan::Branch::Reached(const Actor& actor,const Position& target) const
    {
        return target == ExpectedState(actor).position;
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
            if (node->ExepctedState(actor).position == state.position)
                return true;
        return false;
    }

    Plan::Node& Plan::Node::operator= (Plan::Node&& other)
    {
        action = std::move(other.action);
        result = other.result;
        return *this;
    }

    void Plan::Add(Game& game, std::unique_ptr<Action> action, const GameChances& outcomes)
    {
        actions.emplace_back(Node(std::move(action), outcomes));
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
            return actions.back().result.front().state;
    }
    void Plan::Execute(Game& game) const
    {
        auto& finalState = result.front(); // todo: compute chance
        OutputDebugStringW((Description() + L" " + result.front().description+ L" = " + finalState.Description() + L"\r\n").c_str());
        finalState.Apply();
    }


    void Plan::Approach(const Position& target, Game& game, std::unique_ptr<Action>&& targetAction)
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
        if (first->Reached(actor, target))
            return;
        open.emplace(std::move(first));
        while (!open.empty())
        {
            auto best = open.begin();

            if (targetAction)
            {
                auto outcomes = targetAction->Act((*best)->ExpectedState(actor), game);
                if (outcomes.size()>0)
                {
                    AddFront(**best);
                    for (Branch* previous = (*best)->previous; previous != nullptr; previous = previous->previous)
                    {
                        AddFront(*previous);
                    }
                    Add(game, std::move(targetAction), outcomes);   // does it matter which outcome's state?
                    return;
                }
            }
            else
            {
                if ((*best)->Reached(actor, target))
                {
                    auto gameState = std::make_unique<GameState>(game);
                    gameState->Adjust(actor, (*best)->ExpectedState(actor));
                    result.emplace_back(GameChance(std::move(gameState), 1.0, L"Move"));

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
                auto outcomes = action->Act((*bestIt.first)->ExpectedState(actor), game);
                if (outcomes.size()==0)
                    continue;
                auto newState = outcomes.front().state;
                bool alreadyClosed = closed.Contains(newState);
                if (alreadyClosed)    // TODO if new score < closed, still allow? is this possible?
                    continue;
                auto newNode = std::make_unique<Branch>(*(*bestIt.first), std::move(action), outcomes);
                auto newIt = open.emplace(std::move(newNode));
                assert(newIt.second);
            }
        }
        auto best = closed.begin();
        if ((*best)->action)    // TODO: root node
        {
            result = (*best)->result;
            AddFront(**best);
            for (auto link = (*best)->previous; link != nullptr; link = link->previous)
            {
                if (link->action)
                    AddFront(*link);
            }
        }
    }

    PathPlan::PathPlan(Actor& actor, const Position& target, Game& game) :
        Plan(actor),
        target(target)
    {
        Approach(target, game, nullptr);
    }

    std::wstring PathPlan::Description() const
    {
        return actor.name + L": " + std::wstring(L"Move to ") + target.Description();
    }

    AttackPlan::AttackPlan(Actor& actor, Actor& target, Game& game, const Skill& skill) :
        Plan(actor),
        skill(skill),
        target(target)
    {
        Approach(target.GetPosition(), game, std::unique_ptr<Action>(skill.Action(target)));
    }

    std::wstring AttackPlan::Description() const
    {
        return actor.name + L": " + skill.name + L" @ " + target.name;
    }

    ManualPlan::ManualPlan(Actor& actor) :
        Plan(actor)
    {
    }
    
    std::wstring ManualPlan::Description() const
    {
        if (actions.empty())
            return actor.name + L": idle";
        else
        {
            std::wstring result(actor.name + L": ");
            for (const auto& node : actions)
            {
                result += node.action->Description() + L", ";
            }
            return result;
        }
    }


}
