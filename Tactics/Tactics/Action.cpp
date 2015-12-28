#include "stdafx.h"
#include "Action.h"
#include "Actor.h"
#include "Game.h"

namespace Game
{
	Result::Result(const Actor& actor) :
		possible(false),
		position(actor.GetPosition()),
		actionPoints(actor.GetActionPoints())
	{
	}

	Result North::Act(const Actor& actor, const Game& game)
	{
		Result result(actor);
		result.position.y += 1;
		result.possible = game.CanBe(result.position);
		return result;
	}

	Result East::Act(const Actor& actor, const Game& game)
	{
		Result result(actor);
		result.position.x += 1;
		result.possible = game.CanBe(result.position);
		return result;
	}

	Result South::Act(const Actor& actor, const Game& game)
	{
		Result result(actor);
		result.position.y -= 1;
		result.possible = game.CanBe(result.position);
		return result;
	}

	Result West::Act(const Actor& actor, const Game& game)
	{
		Result result(actor);
		result.position.x -= 1;
		result.possible = game.CanBe(result.position);
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