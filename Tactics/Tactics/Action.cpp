#include "stdafx.h"
#include "Action.h"
#include "Actor.h"
#include "Game.h"
#include "Direction.h"
namespace Game
{
	Result::Result(const Actor& actor) :
		possible(false),
		position(actor.GetPosition()),
		actionPoints(actor.GetActionPoints())
	{
	}

	Move::Move(Direction direction) :
		direction(direction)
	{
	}

	North::North() :
		Move(Direction::Value::North)
	{
	}
	
	Result North::Act(const Actor& actor, const Game& game)
	{
		Result result(actor);
		result.position += direction.Vector();
		result.possible = game.CanBe(result.position) &&
			game.CanGo(actor.GetPosition(), direction);

		return result;
	}

	East::East() :
		Move(Direction::Value::East)
	{
	}

	Result East::Act(const Actor& actor, const Game& game)
	{
		Result result(actor);
		result.position += direction.Vector();
		result.possible = game.CanBe(result.position) &&
			game.CanGo(actor.GetPosition(), direction);
		return result;
	}

	South::South() :
		Move(Direction::Value::South)
	{
	}

	Result South::Act(const Actor& actor, const Game& game)
	{
		Result result(actor);
		result.position += direction.Vector();
		result.possible = game.CanBe(result.position) &&
			game.CanGo(actor.GetPosition(), direction);
		return result;
	}

	West::West() :
		Move(Direction::Value::West)
	{
	}
	Result West::Act(const Actor& actor, const Game& game)
	{
		Result result(actor);
		result.position += direction.Vector();
		result.possible = game.CanBe(result.position) &&
			game.CanGo(actor.GetPosition(), direction);
		return result;
	}


	std::map<unsigned, std::function<Action*(void)>> Action::keymap = 
	{
		{ VK_UP, [](){ return new North(); } },
		{ VK_RIGHT, [](){ return new East(); } },
		{ VK_DOWN, [](){ return new South(); } },
		{ VK_LEFT, [](){ return new West(); } },
	};
} // ::Game