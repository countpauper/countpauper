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

	Plan::Node::Node(Node& previous, std::unique_ptr<Action>&& _action) :
		previous(&previous),
		state(std::make_unique<GameState>(*previous.state)),
		action(std::move(_action)),
		chance(double(action->Chance(*previous.state).Value())/100.0)
	{
		action->Act(*state);
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
        OutputDebugStringW((L"Execute: " + Description() + L" Fail\r\n").c_str());
        return false;
    }

    void Plan::Apply(const GameState& state, Game& game) const
    {
        OutputDebugStringW((L"Execute: " + Description() + L" " + state.Description() + L"\r\n").c_str());
        state.Apply(game);

    }

	Anatomy HitLocation(const State& attacker, const Skill& skill, const State& victim, Trajectory trajectory)
	{
		assert(!attacker.direction.Prone());  // prone not supported yet here
		if (victim.direction.Prone())
		{
			return Anatomy();
		}
		auto targeting = skill.target;
		if (targeting.count(Targeting::Swing))
		{
			Anatomy origin = attacker.Origin(skill, trajectory);
			// TODO: mirror for left arm/tail
			Anatomy facing(attacker.direction, victim.direction, 0);    // TODO: sensing body part or height difference
			return Anatomy(origin, facing);
		}
		else if (targeting.count(Targeting::Center))
		{
			auto height = victim.body.Length();
			double middle = static_cast<double>(height) / 2.0;
			assert(false);	// random state storage is removed, need to reimplement center mass
			auto hitHeight = static_cast<int>(std::round(middle + Engine::Random().Normal(1.0)));
			if (hitHeight < 0 || hitHeight >= static_cast<int>(height))
				return Anatomy();
			return Anatomy(attacker.direction, victim.direction, hitHeight);
		}
		else if (targeting.count(Targeting::Intercept))
		{
			Anatomy origin = attacker.Origin(skill, trajectory);
			// TODO: mirror for left arm/tail
			Anatomy facing(attacker.direction, victim.direction, 0);    // TODO: sensing body part or height difference
			return Anatomy(origin, facing);
		}
		assert(false);  // unsupported for now
		return Anatomy();
	}

	const Body::Part* Aim(const IGame& state, const Actor& actor, const Actor& target, const Skill& skill)
	{
		State attacker(state.Get(actor));
		State victim(state.Get(target));
		for (auto trajectory : skill.trajectory)
		{
			auto location = HitLocation(attacker, skill, victim, trajectory);
			if (!location)
				continue;
			auto part = victim.body.Get(location);
			if (part)
				return part;
		}
		return nullptr;
	}

	bool Plan::PlanAction(Node& parent, const Skill& skill, const Actor& actor, const Target& target)
    {
		auto& state = parent.state->Get(actor);
		if (!state.IsPossible(skill, target))
			return false;

		auto part = Aim(*parent.state, actor, dynamic_cast<const Actor&>(target), skill);
		if (!part && !skill.trajectory.empty())
			return false;

		std::unique_ptr<TargetedAction> action(skill.CreateAction(actor, target, part));
		if (!action)
			return false;

		auto node = std::make_unique<Node>(parent, std::move(action));

        auto targetActor = dynamic_cast<const Actor*>(&target);
        if ((targetActor) &&
			(!targetActor->IsAlly(actor)))	// TODO: target aware of actor and action
        {
            auto defenses = targetActor->Counters(skill);
            for (auto defense : defenses)
            {
                PlanReaction(parent, *defense, *targetActor, dynamic_cast<TargetedAction&>(*node->action));
            }
        }
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

    bool Plan::PlanReaction(Node& parent, const Skill& skill, const Actor& defender, const TargetedAction& offense)
    {
		auto state = parent.state->Get(defender);
		if (!state.IsPossible(skill, offense))
			return false;

		auto part = Aim(*parent.state, defender, offense.actor, skill);
		if (!part && !skill.trajectory.empty())
			return false;
		std::unique_ptr<TargetedAction> reaction(skill.CreateAction(defender, offense, part));
		auto node = std::make_unique<Node>(parent, std::move(reaction));
		parent.children.emplace_back(std::move(node));
		PlanCombo(*node, skill, defender, offense.actor);
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
            for (auto& action : bestNode.state->PossibleMoves())
            {
				auto newNode = std::make_unique<Node>(bestNode, std::move(action));
                bool alreadyClosed = closed.Contains(*newNode->state);
                if (!alreadyClosed)    // TODO if new score < closed, still allow? is this possible?
                {
                    auto newIt = open.emplace(std::move(newNode));
                    assert(newIt.second);
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
            for (auto& action : bestNode.state->PossibleMoves())
            {
				auto newNode = std::make_unique<Node>(bestNode, std::move(action));
                bool alreadyClosed = closed.Contains(*newNode->state);
                if (!alreadyClosed)    // TODO if new score < closed, still allow? is this possible?
                {
                    auto newIt = open.emplace(std::move(newNode));
                    assert(newIt.second);
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
        OutputDebugStringW((L"Execute: " + Description() + L"\r\n").c_str());
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
