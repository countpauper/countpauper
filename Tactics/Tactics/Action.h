#pragma once

#include <map>
#include <functional>
#include "Direction.h"
#include "State.h"

namespace Game
{
	class Actor;
	class Game;

	class Action
	{
	public:
		Action();
		virtual State Act(const State& state, const Game& game)=0;
		virtual void Render(const State& state) const = 0;
		static std::map<unsigned, std::function<Action*(void)>> keymap;
	protected:
		unsigned cost;
	};

	class Move :
		public Action
	{
	public:
		Move(Direction direction);
		void Render(const State& state) const override;
		State Act(const State& state, const Game& game) override;
	protected:
		Direction direction;
	};

	class North : public Move
	{
	public:
		North();
	};
	class East : public Move
	{
	public:
		East();
	};
	class South : public Move
	{
	public:
		South();
	};
	class West : public Move
	{
	public:
		West();
	};


}   // ::Game

