#include "stdafx.h"
#include <sstream>
#include <numeric>
#include "Future.h"
#include "State.h"
#include "Actor.h"
#include "Action.h"
#include "Skills.h"

namespace Game
{
Future::Future(const IGame& parent) :
	Future(parent, *parent.Executor())
{
}

Future::Future(const IGame& parent, const Identity& executor) :
	parent(parent),
	executor(executor)
{
}
void Future::Adjust(const Identity& actor, const State& actorState, const std::wstring& adjustedDescription)
{
	state[&actor] = actorState;
	description[&actor] += adjustedDescription + L" ";
}

State Future::Get(const Identity& actor) const
{
	const auto actorIt = state.find(const_cast<Identity*>(&actor));
	if (actorIt != state.end())
		return actorIt->second;
	return parent.Get(actor);
}

void Future::Apply(IGame& root) const
{
	parent.Apply(root);

	for (auto& actorState : state)
	{
		auto describeIt = description.find(actorState.first);
		auto describe = (describeIt == description.end()) ? L"" : describeIt->second;
		root.Adjust(*actorState.first, actorState.second, describe);
	}
}

const Identity* Future::Executor() const
{
	return &executor;
}

State Future::ActorState() const
{
	return Get(executor);
}

bool Future::HasParent(const IGame& state) const
{
	if (&state == this)
		return true;
	if (auto parentState = dynamic_cast<const Future*>(&parent))
	{
		return parentState->HasParent(state);
	}
	else
	{
		return false;
	}
}


bool Future::CanBe(const Position& position) const
{
	return parent.CanBe(position);
}

bool Future::CanGo(const Position& from, Direction direction) const
{
	return parent.CanGo(from, direction);
}

bool Future::Cover(const Position& from, const Position& to) const
{
	return parent.Cover(from, to);
}

std::vector<std::unique_ptr<Action>> Future::PossibleMoves() const
{
	auto& actorState = ActorState();

	std::vector<std::unique_ptr<Action>> result;
	for (auto skill : actorState.knowledge)
	{
		if ((skill->IsMove()) &&
			(actorState.body.Ready(*skill)))
		{
			int range = (2 + skill->range) / 3;	// convert from El to squares approximately, IsPossible range check will filter
			for (int y = -range; y <= range; ++y)
			{
				for (int x = -range; x <= range; ++x)
				{
					Position vector(x, y);
					if (!vector)
						continue;
					Destination destination(actorState.position + vector);
					if (!CanBe(destination.GetPosition()))
						continue;
					if (actorState.IsPossible(*skill, destination))
					{
						assert(skill->trajectory.empty());	// "aimed" move (eg jump, teleport() not yet implemented
						result.emplace_back(skill->CreateAction(*Executor(), destination, nullptr));
					}
				}
			}
		}
	}
	return result;
}


std::wstring Future::Description() const
{
	std::wstringstream ss;
	for (const auto& actorState : state)
	{
		ss << actorState.first->Description() << L": " << actorState.second.body.Description() << L", "; // L" @ (" << actorState.second.position.Description() << L"), ";
	}
	return ss.str();
}
} // ::Game
