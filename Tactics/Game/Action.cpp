#include "stdafx.h"
#include "Action.h"
#include "Move.h"
#include "Attack.h"
#include "Game.h"
#include "Game/Skills.h"
#include "State.h"
#include "Actor.h"

namespace Game
{

    Action::Result::Result() :
        chance(0.0)
    {
    }

    Action::Result::Result(const IGame& parent, const Actor& actor) :
        state(std::make_unique<GameState>(parent, actor)),
        chance(1.0)
    {
    }

    Action::Result::Result(Action::Result&& other) :
        state(std::move(other.state)),
        chance(other.chance),
        description(other.description)
    {
        other.chance = 0.0;
    }

    Action::Result& Action::Result::operator=(Action::Result&& other)
    {
        state = std::move(other.state);
        chance = other.chance;
        other.chance = 0.0;
        description = other.description;
        return *this;
    }

    Action::Result::operator bool() const
    {
        return state != nullptr;
    }

    Action::Action(const Skill& skill, const Actor& actor) :
        skill(skill),
        actor(actor)
    {
    }


	std::wstring Action::Description() const
	{
		return skill.name;
	}
	
	TargetedAction::TargetedAction(const Skill& skill, const Actor& actor, const Target& target) :
        Action(skill, actor),
        target(target)
    {
    }

	Action::Result TargetedAction::Fail(const IGame& game, const std::wstring& reason) const
	{
		State state = game.Get(actor);
		Result ret(game, actor);
		state.Spent(skill.mp);
		state.Engage(skill);
		ret.state->Adjust(actor, state);
		ret.description = actor.Description() + L" " + skill.name + L" " + reason + L" " + target.Description();
		return std::move(ret);
	}

	Position TargetedAction::GetPosition() const
	{
		return target.GetPosition();
	}

	std::wstring TargetedAction::Description() const
	{
		return skill.name + L"@" +target.Description();
	}

	AimedAction::AimedAction(const Skill& skill, const Actor& actor, const Target& target, const Body::Part& part) :
		TargetedAction(skill, actor, target),
		part(part)
	{

	}

	std::map<unsigned, std::function<Action*(const State& state, const Game& game)>> Action::keymap =
    {};
    /*
        { VK_UP, [](const State&, const Game& game){ return new South(*game.ActiveActor()); } },
        { VK_RIGHT, [](const State&, const Game& game){ return new East(*game.ActiveActor()); } },
        { VK_DOWN, [](const State&, const Game&  game){ return new North(*game.ActiveActor()); } },
        { VK_LEFT, [](const State&, const Game& game){ return new West(*game.ActiveActor()); } },
        { VK_SPACE, [](const State& state, const Game& game)
        {
            auto& skill = game.skills.front();
            const auto& actor = *game.ActiveActor();
            auto targets = game.FindTargetsInRange(state, skill);
            if (targets.empty())
                return (Action*)nullptr;
            return skill.CreateAction(actor, *targets.front());
        } },
    };
    */
} // ::Game