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
        Node* node = m_root.get();
        while (node)
        {
            node->Render();
            node = node->Next();
        }
    }

    Plan::Node::Node(IGame& state) :
        previous(nullptr),
        state(std::make_unique<GameState>(state)),
        chance(1.0)
    {
    }

    Plan::Node::Node(Node& previous, std::unique_ptr<GameState>&& state, std::unique_ptr<Action>&& action) :
        previous(&previous),
        action(std::move(action)),
        state(std::move(state)),
        chance(1.0)
    {
    }

    Plan::Node::~Node()
    {
    }
    bool Plan::Node::DeadEnd() const
    {
        return !state.get();
    }

    bool Plan::Node::IsRoot() const
    {
        return previous == nullptr;
    }

    Plan::Node* Plan::Node::Next() const
    {
        if (children.empty())
            return nullptr;
        else
            return children.back().get();
    }

    void Plan::Node::Render() const
    {
        State actor(state->ActorState());
        Node* prev = previous;
        if (prev)
            actor = prev->state->ActorState();
        if (action)
            action->Render(actor);
    }
 

    bool Plan::Node::Compare(const Node& other, const Position& target) const
    {
        auto& actorState = state->ActorState();
        auto& otherState = other.state->ActorState();
        if (target.ManDistance(actorState.position) < target.ManDistance(otherState.position))
            return true;
        else if (target.ManDistance(actorState.position) > target.ManDistance(otherState.position))
            return false;
        else if (actorState.mp > otherState.mp)
            return true;
        else if (actorState.mp < otherState.mp)
            return false;
        return
            this < &other;
    }
        
    Plan::NodeCompare::NodeCompare(const Position& target) :
        target(target)
    {
    }

    bool Plan::NodeCompare::operator() (const std::unique_ptr<Node>& a, const std::unique_ptr<Node>& b) const
    {
        return a->Compare(*b, target);
    }

    bool Plan::Node::Reached(const Position& target) const
    {
        return target == state->ActorState().position;
    }

    Plan::OpenTree::OpenTree(const Position& target) :
        std::set<std::unique_ptr<Node>, NodeCompare>(NodeCompare(target))
    {
    }

    std::unique_ptr<Plan::Node> Plan::OpenTree::Pop()
    {
        auto& ptr = const_cast<std::unique_ptr<Node>&>(*begin());
        auto result = std::move(ptr);
        erase(begin());
        return result;
    }

    Plan::ClosedList::ClosedList(const Position& target) :
        std::set<std::unique_ptr<Node>, NodeCompare>(NodeCompare(target))
    {
    }


    bool Plan::ClosedList::Contains(const GameState& findState) const
    {
        for (const auto& node : *this)
            if (node->state->ActorState().position == findState.ActorState().position)
                return true;
        return false;
    }

    std::unique_ptr<Plan::Node> Plan::ClosedList::Extract(Node* node)
    {
        auto it = std::find_if(begin(), end(), [node](const value_type& v)
        {
            return v.get() == node;
        });
        auto& ptr = const_cast<std::unique_ptr<Node>&>(*it);
        auto result = std::move(ptr);
        erase(it);
        return result;
    }

    std::unique_ptr<Plan::Node> Plan::ClosedList::ExtractRoot(std::unique_ptr<Node>& leaf)
    {
        auto node = std::move(leaf);
        while (!node->IsRoot())
        {
            auto previous = node->previous;
            node->previous->children.emplace_back(std::move(node));
            node= Extract(previous);
        }
        return std::move(node);
    }

    bool Plan::Valid() const
    {
        return m_root!=nullptr;
    }

    GameChances Plan::Node::AllOutcomes() const
    {
        GameChances ret;
        if (children.empty())
        {
            if (state)
            {
                ret.emplace_back(std::make_pair(chance, state.get()));
            }
        }
        else
        {
            for (auto& child : children)
            {
                auto childOutcomes = child->AllOutcomes();
                ret.insert(ret.end(), childOutcomes.begin(), childOutcomes.end());
            }
        }
        return ret;
    }

    GameChances Plan::AllOutcomes() const
    {
        return m_root->AllOutcomes();
    }

    void Plan::Execute(Game& game) const
    {
        auto outcomes = AllOutcomes();
        for (auto outcome : outcomes)
        {
            double score = rand() / double(RAND_MAX);
            if (score < outcome.first)
            {
                OutputDebugStringW((Description() + L" " + outcome.second->Description() + L"\r\n").c_str());
                outcome.second->Apply(game);
                return;
            }
        }
        OutputDebugStringW((Description() + L" fizzle \r\n").c_str());
    }

    bool Plan::PlanAction(Node& parent, const Skill& skill, const Actor& actor, const Actor& target)
    {
        std::unique_ptr<Action> action(skill.CreateAction(actor, target));
        auto result = action->Act(*parent.state);
        if (!result)
            return false;

        auto defences = target.FollowSkill(skill, Skill::Trigger::Defend);
        for (auto defence : defences)
        {
            PlanAction(parent, *defence, target, actor);
        }

        auto node = std::make_unique<Node>(parent, std::move(result), std::move(action));
        node->chance = skill.GetChance(actor);
        auto combos = node->state->ActiveActor()->FollowSkill(skill, Skill::Trigger::Combo);
        for (auto combo : combos)
        {
            if (PlanAction(*node, *combo, actor, target))
                break;
        }
        parent.children.emplace_back(std::move(node));
        return true;
    }

    void Plan::Approach(const Actor& target, Game& game, const Skill& skill)
    {
        const auto& actor = *game.ActiveActor();

        auto targetPosition = target.GetPosition();
        OpenTree open(targetPosition);
        ClosedList closed(targetPosition);
        auto first = std::make_unique<Node>(game);
        if (first->Reached(targetPosition))
            return;
        open.emplace(std::move(first));
        while (!open.empty())
        {
            std::unique_ptr<Node> best = open.Pop();
            auto act = PlanAction(*best, skill, *best->state->ActiveActor(), target);
            if (act)
            {
                m_root = closed.ExtractRoot(std::move(best));
                return;
            }
            auto bestIt = closed.emplace(std::move(best));
            assert(bestIt.second);
            auto& bestNode = **bestIt.first;
            for (auto& action : actor.AllMoves(bestNode.state->ActorState().position))
            {
                auto result = action->Act(*bestNode.state);
                if (result)
                {
                    bool alreadyClosed = closed.Contains(*result);
                    if (!alreadyClosed)    // TODO if new score < closed, still allow? is this possible?
                    {
                        auto newNode = std::make_unique<Node>(bestNode, std::move(result), std::move(action));
                        auto newIt = open.emplace(std::move(newNode));
                        assert(newIt.second);
                    }
                }
            }
        }
    }

    // TODO: Refactor to avoid code duplication with Approach
    void Plan::Goto(const Position& targetPosition, Game& game)
    {
        const auto& actor = *game.ActiveActor();
        OpenTree open(targetPosition);
        ClosedList closed(targetPosition);
        open.emplace(std::make_unique<Node>(game));
        while (!open.empty())
        {
            std::unique_ptr<Node> best = open.Pop();

            if (best->Reached(targetPosition))
            {
                m_root = closed.ExtractRoot(std::move(best));
                return;
            }
            auto bestIt = closed.emplace(std::move(best));
            assert(bestIt.second);
            auto& bestNode = **bestIt.first;
            for (auto& action : actor.AllMoves(bestNode.state->ActorState().position))
            {
                auto result = action->Act(*bestNode.state);
                if (result)
                {
                    bool alreadyClosed = closed.Contains(*result);
                    if (!alreadyClosed)    // TODO if new score < closed, still allow? is this possible?
                    {
                        auto newNode = std::make_unique<Node>(bestNode, std::move(result), std::move(action));
                        auto newIt = open.emplace(std::move(newNode));
                        assert(newIt.second);
                    }
                }
            }
        }
    }


    PathPlan::PathPlan(Actor& actor, const Position& target, Game& game) :
        Plan(actor),
        target(target)
    {
        Goto(target, game);
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
        Approach(target, game, skill);
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
        if (!m_root)
            return actor.name + L": idle";
        else
        {
            std::wstring result(actor.name + L": ");
            auto node = m_root.get();
            result += node->action->Description();
            while (!node->children.empty())
            {
                node = node->children.front().get();
                result += L", " + node->action->Description() ;
            }
            return result;
        }
    }
}
