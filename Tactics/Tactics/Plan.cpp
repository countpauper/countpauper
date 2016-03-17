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
            state = node.ExpectedState().ActorState();
        }
    }

    Plan::Node::Node(Node&& other) :
        state(other.state),
        action(std::move(other.action)),
        result(other.result)
    {
    }

    Plan::Node& Plan::Node::operator=(Node&& other)
    {
        state = other.state;
        action = std::move(other.action);
        result = other.result;
        return *this;
    }

    Plan::Node::Node(IGame& state) :
        state(state)
    {
    }

    Plan::Node::Node(IGame& state, std::unique_ptr<Action> action) :
        state(state),
        action(std::move(action)),
        result(action->Act(state))
    {
    }

    const GameState& Plan::Node::ExpectedState() const
    {
        return result.front();
    }

    GameState& Plan::Node::ExpectedState()
    {
        return result.front();
    }

    Plan::Branch::Branch(IGame& state) :
        Node(state),
        previous(nullptr)
    {
    }

    Plan::Branch::Branch(Branch& previous, IGame& state, std::unique_ptr<Action> action) :
        Node(state, std::move(action)),
        previous(&previous)
    {
    }

    bool Plan::Branch::Compare(const Branch& other, const Position& target) const
    {
        auto& actorState = ExpectedState().ActorState();
        auto& otherSate = other.ExpectedState().ActorState();
        if (target.ManDistance(actorState.position) < target.ManDistance(otherSate.position))
            return true;
        else if (target.ManDistance(actorState.position) > target.ManDistance(otherSate.position))
            return false;
        else if (actorState.mp > otherSate.mp)
            return true;
        else if (actorState.mp < otherSate.mp)
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
        return target == ExpectedState().ActorState().position;
    }

    Plan::OpenTree::OpenTree(const Position& target) :
        std::set<std::unique_ptr<Branch>, BranchCompare>(BranchCompare(target))
    {
    }

    Plan::ClosedList::ClosedList(const Position& target) :
        std::set<std::unique_ptr<Branch>, BranchCompare>(BranchCompare(target))
    {
    }


    bool Plan::ClosedList::Contains(const GameState& findState) const
    {
        for (const auto& node : *this)
            if (node->ExpectedState().ActorState().position == findState.ActorState().position)
                return true;
        return false;
    }

    void Plan::Add(IGame& game, std::unique_ptr<Action> action)
    {
        actions.emplace_back(Node(game, std::move(action)));
    }
    void Plan::AddFront(Node& node)
    {
        if (!node.action)
            return; // TODO: better way to not add root node
        actions.emplace(actions.begin(), std::move(Node(node.state, std::move(node.action))));
    }

    State Plan::Final() const
    {
        if (actions.empty())
            return State(actor);
        else
            return actions.back().ExpectedState().ActorState();
    }
    void Plan::Execute(Game& game) const
    {
        auto& finalState = actions.back().result.front(); // todo: compute chance, flatten all outcomes
        OutputDebugStringW((Description() + L" " + finalState.description + L" = " + finalState.Description() + L"\r\n").c_str());
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
        auto first = std::make_unique<Branch>(game);
        if (first->Reached(target))
            return;
        open.emplace(std::move(first));
        while (!open.empty())
        {
            auto best = open.begin();

            if (targetAction)
            {
                auto outcomes = targetAction->Act((*best)->result.front());
                if (outcomes.size()>0)
                {
                    AddFront(**best);
                    for (Branch* previous = (*best)->previous; previous != nullptr; previous = previous->previous)
                    {
                        AddFront(*previous);
                    }
                    Add((*best)->state, std::move(targetAction));   // does it matter which outcome's state?
                    return;
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

            auto bestIt = closed.emplace(std::make_unique<Branch>(*(*best)->previous, (*best)->ExpectedState(), std::move((*best)->action)));
            assert(bestIt.second);
            open.erase(best);
            for (const auto& actionFactory : actions)
            {
                std::unique_ptr<Action> action(actionFactory());
                auto outcomes = action->Act((*bestIt.first)->result.front());
                if (outcomes.size()==0)
                    continue;
                auto newState = outcomes.front();
                bool alreadyClosed = closed.Contains(newState);
                if (alreadyClosed)    // TODO if new score < closed, still allow? is this possible?
                    continue;
                auto newNode = std::make_unique<Branch>(**bestIt.first, (*bestIt.first)->ExpectedState(), std::move(action));
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
