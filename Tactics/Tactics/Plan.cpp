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
    Plan::Plan(const Actor& actor) :
        actor(actor)
    {
    }

    void Plan::Render() const
    {
        State state(actor);
        Node* node = root.get();
        while (node)
        {
            node->Render();
            node = node->Next();
        }
    }

    Plan::Node::Node(const IGame& state) :
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

    Plan::WaitTrigger::WaitTrigger(const Actor& target) :
        actor(target)
    {
    }
    bool Plan::WaitTrigger::Anticipating(const Target& target) const
    {
        return &target == &actor;
    }


    bool Plan::Anticipating() const
    {
        return trigger != nullptr;
    }
    bool Plan::Anticipating(const Target& target) const
    {
        if (!trigger)
            return false;
        return trigger->Anticipating(target);
    }

    bool Plan::Valid() const
    {
        return root!=nullptr;
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
        if (root)
            return root->AllOutcomes();
        else
            return GameChances();
    }

    std::vector<Action*> Plan::ActionSequence(GameState& end) const
    {
        std::vector<Action*> sequence;
        auto node = root.get();
        while (node)
        {
            sequence.emplace_back(node->action.get());
            Node* next = nullptr;
            for (const auto& child : node->children)
            {
                if (!end.HasParent(*child->state))
                    continue;
                next = child.get();
                break;
            }
            node = next;
        }
        return sequence;
    }

    bool Plan::Execute(Game& game) const
    {
        auto outcomes = AllOutcomes();
        for (auto outcome : outcomes)
        {
            double score = rand() / double(RAND_MAX);
            if (score < outcome.first)
            {
                auto actionSequence = ActionSequence(*outcome.second);
                for (auto action : actionSequence)
                {
                    if (action)
                        OutputDebugStringW((action->Description() + L", ").c_str());
                }
                OutputDebugStringW((Description() + L" " + outcome.second->Description() + L"\r\n").c_str());
                outcome.second->Apply(game);
                return true;
            }
        }
        OutputDebugStringW((Description() + L" fizzle \r\n").c_str());
        return false;
    }

    bool Plan::PlanAction(Node& parent, const Skill& skill, const Actor& actor, const Target& target)
    {
        std::unique_ptr<Action> action(skill.CreateAction(actor, target));
        auto result = action->Act(*parent.state);
        if (!result)
            return false;

        auto targetActor = dynamic_cast<const Actor*>(&target);
        if (targetActor)
        {
            auto defences = targetActor->FollowSkill(skill, Skill::Trigger::Defend);
            for (auto defence : defences)
            {
                PlanAction(parent, *defence, *targetActor, actor);
            }
        }
        auto node = std::make_unique<Node>(parent, std::move(result), std::move(action));
        const auto& actorState = node->state->Get(actor);
        node->chance = double(actorState.DefendChance(skill).Value()) / 100.0;
        auto combos = actor.FollowSkill(skill, Skill::Trigger::Combo);
        for (auto combo : combos)
        {
            if (PlanAction(*node, *combo, actor, target))
                break;
        }
        parent.children.emplace_back(std::move(node));
        return true;
    }

    std::unique_ptr<Plan::Node> Plan::Approach(const Target& target, const Game& game, const Skill& skill)
    {
        assert(game.SelectedActor() == &actor); // Nodes assume the currently selected actor is doing the planning
        auto targetPosition = target.GetPosition();
        OpenTree open(targetPosition);
        ClosedList closed(targetPosition);
        auto first = std::make_unique<Node>(game);

        open.emplace(std::move(first));
        while (!open.empty())
        {
            std::unique_ptr<Node> best = open.Pop();
            auto act = PlanAction(*best, skill, actor, target);
            if (act)
            {
                return closed.ExtractRoot(std::move(best));
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
        return nullptr;
    }

    // TODO: Refactor to avoid code duplication with Approach
    std::unique_ptr<Plan::Node> Plan::Goto(const Position& targetPosition, const Game& game)
    {
        OpenTree open(targetPosition);
        ClosedList closed(targetPosition);
        open.emplace(std::make_unique<Node>(game));
        while (!open.empty())
        {
            std::unique_ptr<Node> best = open.Pop();

            if (best->Reached(targetPosition))
            {
                return closed.ExtractRoot(std::move(best));
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
        return nullptr;
    }

    WaitPlan::WaitPlan(const Actor& actor, const Target& target, const Game& game) :
        Plan(actor),
        target(target)
    {
        Compute(game);
        trigger = std::make_unique<WaitTrigger>(actor);
    }

    void WaitPlan::Compute(const Game& game)
    {
        root = std::make_unique<Node>(game);
    }

    std::wstring WaitPlan::Description() const
    {
        return actor.Description() + L": " + std::wstring(L"Wait for ") + target.Description();
    }

    SkipPlan::SkipPlan(const Actor& actor, const Game& game) :
        Plan(actor)
    {
        Compute(game);
    }

    std::wstring SkipPlan::Description() const
    {
        return actor.Description() + L": " + std::wstring(L"Wait");
    }

    void SkipPlan::Compute(const Game& game)
    {
        root = std::make_unique<Node>(game);
    }

    PathPlan::PathPlan(const Actor& actor, const Position& target, const Game& game) :
        Plan(actor),
        target(target)
    {
        Compute(game);
    }

    void PathPlan::Compute(const Game& game)
    {
        root = Goto(target, game);
    }

    std::wstring PathPlan::Description() const
    {
        return actor.Description() + L": " + std::wstring(L"Move to ") + target.Description();
    }

    AttackPlan::AttackPlan(const Actor& actor, const Target& target, const Game& game, const Skill& skill) :
        Plan(actor),
        skill(skill),
        target(target)
    {
        Compute(game);
    }

    void AttackPlan::Compute(const Game& game)
    {
        root = Approach(target, game, skill);
    }

    std::wstring AttackPlan::Description() const
    {
        return actor.Description() + L": " + skill.name + L" @ " + target.Description();
    }

    ManualPlan::ManualPlan(Actor& actor) :
        Plan(actor)
    {
    }
    
    void ManualPlan::Compute(const Game& game)
    {   // how? only can check if it's still valid
        assert(false);
    }

    std::wstring ManualPlan::Description() const
    {
        if (!root)
            return actor.Description() + L": idle";
        else
        {
            std::wstring result(actor.Description() + L": ");
            auto node = root.get();
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
