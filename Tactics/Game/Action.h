#pragma once

#include <map>
#include <functional>
#include <memory>
#include "Target.h"
#include "Body.h"

namespace Game
{
    class Actor;
    class Game;
    class Skill;
    class IGame;
    class GameState;
    class State;


    class Action
    {
    public:
        Action(const Skill& skill, const Actor& actor);
        virtual ~Action() = default;

        virtual void Act(IGame& game) const = 0;
		virtual void Render(const State& state) const = 0;
		virtual Score Chance(const IGame& game) const = 0;

		virtual std::wstring Description() const  = 0;

		static std::map<unsigned, std::function<Action*(const State& state, const Game& game)>> keymap;
        static std::map<std::wstring, std::function<Action*(const State& state, const Game& game)>> typemap;
        
		const Actor& actor;
        const Skill& skill;
    };
    
	class TargetedAction : public Action, public Target
	{
	public:
		TargetedAction(const Skill& skill, const Actor& actor, const Target& target);
		Score Chance(const IGame& game) const override;
	protected:
		const Target& target;
		Position GetPosition() const override;
		std::wstring Description() const override;	// override Target
	};
	class AimedAction: public TargetedAction
	{
	public:
		AimedAction(const Skill& skill, const Actor& actor, const Target& target, const Body::Part& part);
	protected:
		const Body::Part& part;
	};

}   // ::Game

