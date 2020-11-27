#pragma once

#include <map>
#include <functional>
#include <memory>
#include "Target.h"
#include "Physics/Direction.h"
#include "Body.h"

namespace Game
{
    class Identity;
    class Game;
    class Skill;
    class IGame;
    class State;

    class Action : public Target
    {
    public:
        Action(const Identity& actor, const Skill& skill);
        virtual ~Action() = default;

        virtual void Act(IGame& game) const = 0;
		virtual void Render(const State& state) const = 0;
		virtual Score Chance(const IGame& game) const = 0;

		virtual void Engage(State& engager) const;

		static std::map<unsigned, std::function<Action*(const State& state, const Game& game)>> keymap;
        static std::map<const std::wstring, std::function<Action*(const State& state, const Game& game)>> typemap;
        
		const Identity& actor;
        const Skill& skill;
	
		Position GetPosition() const override;
		std::wstring Description() const override;	// override Target
	};
    
	class TargetedAction : public Action
	{
	public:
		TargetedAction(const Identity& actor, const Skill& skill, const Target& target);
		Score Chance(const IGame& game) const override;
		Position GetPosition() const override;
		std::wstring Description() const override;	// override Target
		void Engage(State& actor) const override;
	protected:
		const Target& target;

	};

	class AimedAction
	{
	public:
		AimedAction(const Part* part);
		const Part* part;
	};

	class DirectedAction

	{
	public:
		DirectedAction(Direction trajectory);
		const Direction trajectory;
	};

}   // ::Game

