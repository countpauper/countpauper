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

	Plan::Link::Link(const State& result) :
		Node(result),
		previous(nullptr)
	{
	}

	Plan::Link::Link(Link& previous, std::unique_ptr<Action> action, const State& state) :
		Node(std::move(action),state),
		previous(&previous)
	{
	}

    Plan::Node::Node(Node&& other) :
		action(std::move(other.action)),
        result(other.result)
    {
    }

	int Plan::Node::Score(const Position& target, unsigned startMovePoints) const
	{
		return target.Distance(result.position) +
			startMovePoints - result.mp;
	}
	bool Plan::Node::Reached(const Position& target) const
	{
		return target == result.position;
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
		typedef std::multimap<int, std::unique_ptr<Link>> OpenTree;
		OpenTree open;
		typedef std::vector<std::unique_ptr<Link>> ClosedList;
		ClosedList closed;
		State start(actor);
		auto first = std::make_unique<Link>(start);
		if (first->Reached(target))
			return;
		open.insert(std::make_pair(first->Score(target, start.mp), std::move(first)));
		while (!open.empty())
		{
			auto best = std::move(open.begin()->second);
			open.erase(open.begin());
			Link& bestNode = *best;
			closed.emplace_back(std::move(best));
			for (const auto& actionFactory : actions)
			{
				std::unique_ptr<Action> action(actionFactory());
				auto newState = action->Act(bestNode.result, game);
				if (!newState.possible)
					continue;
				bool alreadyClosed = false;
				// TODO: make function
				for (const auto& closedNode : closed)
					if (closedNode->result.position == newState.position)
					{
						alreadyClosed = true;
						break;
					}
				if (alreadyClosed)    // TODO if new score < closed, still allow? is this possible?
					continue;
				auto newNode = new Link(bestNode, std::move(action), newState);
				if (newNode->Reached(target))
				{
					AddFront(*newNode);
					for (Link* previous = newNode->previous; previous != nullptr; previous = previous->previous)
					{
						if (previous->action)    // TODO: more gracefull detection of root node
							AddFront(*previous);
					}
					delete newNode;
					return;
				}
				else
				{
					open.insert(std::make_pair(newNode->Score(target, start.mp), std::unique_ptr<Link>(newNode)));
				}
			}
		}
	}

	PathPlan::PathPlan(Actor& actor, const Position& target, const Game& game) :
		Plan(actor),
		target(target)
	{
		Approach(target, game);
	}
}    // ::Game