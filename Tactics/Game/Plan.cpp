#include "stdafx.h"
#include <iostream>
#include "Engine/Random.h"
#include "Game/Skills.h"
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

    Engine::Image Plan::Icon() const
    {
        auto skill = GetSkill();
        return skill->Icon();
    }

    Plan::Node::Node(const IGame& state, const Actor& executor) :
        previous(nullptr),
        state(std::make_unique<GameState>(state, executor)),
        chance(1.0)
    {
    }

    Plan::Node::Node(Node& previous, Action::Result& result, std::unique_ptr<Action>&& action) :
        previous(&previous),
        state(std::move(result.state)),
        action(std::move(action)),
        chance(result.chance),
        description(result.description)
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

    Plan::Wait::Wait(const Actor& target) :
        actor(target)
    {
    }
    bool Plan::Wait::Anticipating(const Target& target) const
    {
        return &target == &actor;
    }


    bool Plan::Anticipating() const
    {
        return condition != nullptr;
    }
    bool Plan::Trigger(const Target& target)
    {
        if (!condition)
            return false;
        if (condition->Anticipating(target))
        {
            condition.reset();
            return true;
        }
        else
            return false;
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
                ret.back().first *= chance;
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

    std::vector<Action*> Plan::ActionSequence(const GameState& end) const
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

    std::vector<std::wstring> Plan::Descriptions(const GameState& end) const
    {
        std::vector<std::wstring> descriptions;
        auto node = root.get();
        while (node)
        {
            descriptions.emplace_back(node->description);
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
        return descriptions;
    }

    bool Plan::Execute(Game& game) const
    {
        auto outcomes = AllOutcomes();
        for (auto outcome : outcomes)
        {
            double score = Engine::Random().Chance();
            if (score < outcome.first)
            {
                Apply(*outcome.second, game);
                return true;
            }
        }
        OutputDebugStringW((Description() + L" fizzle \r\n").c_str());
        return false;
    }

    void Plan::Apply(const GameState& state, Game& game) const
    {
        auto descriptions = Descriptions(state);
        for (auto description : descriptions)
        {
                OutputDebugStringW((description + L"\r\n").c_str());
        }
        OutputDebugStringW((Description() + L" " + state.Description() + L"\r\n").c_str());
        state.Apply(game);

    }
    bool Plan::PlanAction(Node& parent, const Skill& skill, const Actor& actor, const Target& target)
    {
        Action::Result result;
        std::unique_ptr<Action> action;
        for (auto trajectory : skill.trajectory)
        {
            action = std::unique_ptr<Action>(skill.CreateAction(actor, target, trajectory));
            result = action->Act(*parent.state);
            if (result) // TODO: select preferable trajectory instead of first
                break;
        }

        if (!result)
            return false;

        auto targetActor = dynamic_cast<const Actor*>(&target);

        if ((targetActor) &&
			(!targetActor->IsAlly(actor)))	// TODO: target aware of actor and action
        {
            auto defences = targetActor->Counters(skill);
            for (auto defence : defences)
            {
                PlanReaction(parent, skill, action->trajectory, *defence, actor, *targetActor);
            }
        }
        auto node = std::make_unique<Node>(parent, result, std::move(action));

        PlanCombo(*node, skill, actor, target);
        parent.children.emplace_back(std::move(node));
        return true;
    }

    void Plan::PlanCombo(Node& parent, const Skill& skill, const Actor& actor, const Target& target)
    {
        auto combos = actor.Combos(skill);
        for (auto combo : combos)
        {
            PlanAction(parent, *combo, actor, target);
        }
    }

    bool Plan::PlanReaction(Node& parent, const Skill& offense, Trajectory attackTrajectory, const Skill& defense, const Actor& aggressor, const Actor& defender)
    {
        std::unique_ptr<Action> reaction;
        Action::Result intermediate;
        // TODO: select trajectory by opposing attack trajectory. action as argument
        for (auto trajectory : defense.trajectory)
        {
            reaction = std::unique_ptr<Action>(defense.CreateAction(defender, aggressor, trajectory));
            intermediate = reaction->Act(*parent.state);
            if (intermediate)
                break;
        }
        if (!intermediate)
            return false;
        auto node = std::make_unique<Node>(parent, intermediate, std::move(reaction));
        const auto& actorState = parent.state->Get(defender);
        const auto& targetState = parent.state->Get(aggressor);
        node->chance = double(actorState.Chance(defense, targetState).Value()) / 100.0;

        Skill::Effects effects = defense.effects;

        std::unique_ptr<Action> action(offense.CreateAction(aggressor, defender, attackTrajectory));
        Action::Result result;
        if (effects.count(Skill::Effect::Miss) == 0)
        {
            result = action->Act(*node->state);
        }
        else
        {
            result = action->Fail(*node->state, node->action->Description());
        }
        if (result)
        {
            auto defenseNode = std::move(node);
            node = std::make_unique<Node>(*defenseNode, result, std::move(action));
            const auto& actorState = defenseNode->state->Get(aggressor);
            const auto& targetState = defenseNode->state->Get(defender);
            node->chance = double(actorState.Chance(offense, targetState).Value()) / 100.0;
            // TODO: counter reaction? reaction combo (ie disarm)?
            PlanCombo(*node, offense, aggressor, defender);
            defenseNode->children.emplace_back(std::move(node));
            parent.children.emplace_back(std::move(defenseNode));
        }
        else
        {
            // TODO: counter reaction? reaction combo (ie disarm)?
            PlanCombo(*node, offense, aggressor, defender);
            parent.children.emplace_back(std::move(node));
        }
        return true;
    }

    std::unique_ptr<Plan::Node> Plan::Approach(const Target& target, const Game& game, const Skill& skill)
    {
        auto targetPosition = target.GetPosition();
        OpenTree open(targetPosition);
        ClosedList closed(targetPosition);
        auto first = std::make_unique<Node>(game, actor);

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
                    bool alreadyClosed = closed.Contains(*result.state);
                    if (!alreadyClosed)    // TODO if new score < closed, still allow? is this possible?
                    {
                        auto newNode = std::make_unique<Node>(bestNode, result, std::move(action));
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
        open.emplace(std::make_unique<Node>(game, actor));
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
                    bool alreadyClosed = closed.Contains(*result.state);
                    if (!alreadyClosed)    // TODO if new score < closed, still allow? is this possible?
                    {
                        auto newNode = std::make_unique<Node>(bestNode, result, std::move(action));
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
        const auto targetActor = dynamic_cast<const Actor*>(&target);
        if (targetActor)
            condition = std::make_unique<Wait>(*targetActor);
        else
            assert(false);  // wait for area/location
    }

    void WaitPlan::Compute(const Game& game)
    {
        root = std::make_unique<Node>(game, actor);
    }

    const Skill* WaitPlan ::GetSkill() const
    {
        return actor.WaitSkill();
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
        root = std::make_unique<Node>(game, actor);
    }

    const Skill* SkipPlan::GetSkill() const
    {
        return actor.WaitSkill();
    }

    bool SkipPlan::Execute(Game& game) const
    {
        OutputDebugStringW((Description() + L"\r\n").c_str());
        const_cast<Actor&>(actor).Deactivate();
        return true;
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

    const Skill* PathPlan::GetSkill() const
    {
        return actor.DefaultMove();
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

    const Skill* AttackPlan::GetSkill() const
    {
        return &skill;
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

    const Skill* ManualPlan::GetSkill() const
    {
        return actor.DefaultAttack();
    }
}
