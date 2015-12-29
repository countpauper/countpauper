#pragma once

#include "Position.h"
#include <map>
#include <functional>
#include "Direction.h"

namespace Game
{
	class Actor;
	class Game;

	class Result
	{
	public:
		Result(const Actor& actor);
		bool possible;
		Position position;
		unsigned actionPoints;
	};

	class Action
	{
	public:
		Action() = default;
		virtual Result Act(const Actor& actor, const Game& game)=0;
		static std::map<unsigned, std::function<Action*(void)>> keymap;
	};


	class Move :
		public Action
	{
	public:
		Move(Direction direction);
	protected:
		Direction direction;
	};

	class North : public Move
	{
	public:
		North();
		Result Act(const Actor& actor, const Game& game) override;
	};
	class East : public Move
	{
	public:
		East();
		Result Act(const Actor& actor, const Game& game) override;
	};
	class South : public Move
	{
	public:
		South();
		Result Act(const Actor& actor, const Game& game) override;
	};
	class West : public Move
	{
	public:
		West();
		Result Act(const Actor& actor, const Game& game) override;
	};

}   // ::Game

