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
        auto node = m_actions;
        // TODO state is the wrong way around for rendering
        while (node)
        {
            if (node->previous)
                state = node->previous->ExpectedState().ActorState();
            if (node->action)
                node->action->Render(state);
            state = node->ExpectedState().ActorState();
            node = node->previous;
        }
    }

    Plan::Node::Node(IGame& state) :
        state(state)
    {
        result.emplace_back(GameChance(state, 1.0, L"Root"));
    }

    Plan::Node::Node(std::shared_ptr<Plan::Node> previous, IGame& state, std::unique_ptr<Action>&& action) :
        state(state),
        action(std::move(action)),
        result(this->action->Act(state)),
        previous(previous)
    {
    }

    Plan::Node::~Node()
    {
    }
    bool Plan::Node::DeadEnd() const
    {
        return result.empty();
    }

    const GameState& Plan::Node::ExpectedState() const
    {
        return result.front();
    }

    GameState& Plan::Node::ExpectedState()
    {
        return result.front();
    }

    bool Plan::Node::Compare(const Node& other, const Position& target) const
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
        
    Plan::NodeCompare::NodeCompare(const Position& target) :
        target(target)
    {
    }

    bool Plan::NodeCompare::operator() (const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) const
    {
        return a->Compare(*b, target);
    }

    bool Plan::Node::Reached(const Position& target) const
    {
        return target == ExpectedState().ActorState().position;
    }

    Plan::OpenTree::OpenTree(const Position& target) :
        std::set<std::shared_ptr<Node>, NodeCompare>(NodeCompare(target))
    {
    }

    Plan::ClosedList::ClosedList(const Position& target) :
        std::set<std::shared_ptr<Node>, NodeCompare>(NodeCompare(target))
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
        auto node = std::make_shared<Node>(m_actions, game, std::move(action));
        if (!node->DeadEnd())
            m_actions = node;
    }

    bool Plan::Valid() const
    {
        return m_actions.get()!=nullptr;
    }
    State Plan::Final() const
    {
        if (!m_actions)
            return State(actor);
        else
            return m_actions->ExpectedState().ActorState();
    }

    Plan::Outcomes Plan::AllOutcomesRecursive(Node& node) const
    {
        double alternativeScore = node.result.front().chance;
        if (!node.previous)
        {
            Outcomes firstResult;
            for (auto resultIt = node.result.begin() + 1; resultIt != node.result.end(); ++resultIt)
            {
                firstResult[alternativeScore] = &*resultIt;
                alternativeScore += resultIt->chance;
            }
            firstResult[0.0] = &node.result.front();
            return firstResult;
        }
        else
        {
            auto outcomes = AllOutcomesRecursive(*node.previous);
            double remainingscore = 1.0;
            if (outcomes.size() > 1)
            {
                auto it = outcomes.begin()++;
                remainingscore = it->first;
            }
            for (auto resultIt = node.result.begin() + 1; resultIt != node.result.end(); ++resultIt)
            {
                outcomes[alternativeScore * remainingscore] = &*resultIt;
                alternativeScore += resultIt->chance;
            }
            outcomes[0.0] = &node.result.front();
            return outcomes;
        }
    }

    Plan::Outcomes Plan::AllOutcomes() const
    {
        return AllOutcomesRecursive(*m_actions);
    }

    void Plan::Execute(Game& game) const
    {
        auto outcomes = AllOutcomes();
        double score = rand() / double(RAND_MAX);
        auto selectIt = outcomes.lower_bound(score);
        assert(selectIt != outcomes.begin() && "No outcomes");
        selectIt--;
        auto select = selectIt->second;
        OutputDebugStringW((Description() + L" " + select->description + L" = " + select->Description() + L"\r\n").c_str());
        select->Apply();
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
        auto first = std::make_unique<Node>(game);
        if (first->Reached(target))
            return;
        open.emplace(std::move(first));
        while (!open.empty())
        {
            std::shared_ptr<Node> best = *open.begin();
            open.erase(open.begin());

            if (targetAction)
            {
                auto outcomes = targetAction->Act(best->ExpectedState());
                if (outcomes.size())
                {   // TODO find a way to make a Node without moving ownership of the target action
                    auto finalNode = std::make_shared<Node>(best, best->ExpectedState(), std::move(targetAction));
                    assert(!finalNode->DeadEnd());
                    m_actions = finalNode; 
                    return;
                }
            }
            else
            {
                if (best->Reached(target))
                {
                    m_actions = best;
                    return;
                }
            }
   
            auto bestIt = closed.emplace(best);
            assert(bestIt.second);
            for (const auto& actionFactory : actions)
            {
                std::unique_ptr<Action> action(actionFactory());
                auto newNode = std::make_shared<Node>(*bestIt.first, (*bestIt.first)->ExpectedState(), std::move(action));
                if (newNode->DeadEnd())
                    continue;
                auto newState = newNode->ExpectedState();
                bool alreadyClosed = closed.Contains(newState);
                if (alreadyClosed)    // TODO if new score < closed, still allow? is this possible?
                    continue;
                auto newIt = open.emplace(newNode);
                assert(newIt.second);
            }
        }
        m_actions = *closed.begin();
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
        if (!m_actions)
            return actor.name + L": idle";
        else
        {
            std::wstring result(actor.name + L": ");
            auto node = m_actions;
            while (node)
            {
                result += node->action->Description() + L", ";
                node = node->previous;
            }
            return result;
        }
    }


}
