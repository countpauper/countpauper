#include "stdafx.h"
#include "Action.h"
#include "Actor.h"
#include "Game.h"
#include "Direction.h"
namespace Game
{
	State::State(const Actor& actor) :
		possible(false),
		position(actor.GetPosition()),
		mp(actor.GetMovePoints())
	{
	}

	Action::Action() :
		cost(0)
	{
	}

	Move::Move(Direction direction) :
		direction(direction)
	{
		cost = 2;
	}

	State Move::Act(const State& state, const Game& game)
	{
		State result(state);
		if (state.mp <= cost)
		{
			result.possible = false;
			return result;
		}
		result.mp -= cost;
		result.position += direction.Vector();
		result.possible = game.CanBe(result.position) &&
			game.CanGo(state.position, direction);

		return result;
	}

	void Move::Render(const State& state) const
	{
		glColor4ub(255, 255, 255, 255);
		glPushMatrix();
		glTranslatef(float(state.position.x)+0.5f, 0.5f, float(state.position.y) + 0.5f);
		auto v = direction.Vector();
		glBegin(GL_LINES);
			glVertex3f(0, 0, 0);
			glVertex3f(float(v.x), 0, float(v.y));
		glEnd();
		glPopMatrix();
	}
	North::North() :
		Move(Direction::Value::North)
	{
	}
	

	East::East() :
		Move(Direction::Value::East)
	{
	}

	South::South() :
		Move(Direction::Value::South)
	{
	}
	West::West() :
		Move(Direction::Value::West)
	{
	}


	Plan::Plan(const Actor& actor) :
		start(actor)
	{
	}
	void Plan::Render() const
	{
		State state = start;
		for (const auto& node : actions)
		{
			node.action->Render(state);
			state = node.result;
		}
	}

	Plan::Node::Node(std::unique_ptr<Action> action, const State& state) :
		action(std::move(action)),
		result(state)
	{
	}

	Plan::Node::Node(Node&& other) :
		action(std::move(other.action)),
		result(other.result)
	{
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
	void Plan::AddFront(std::unique_ptr<Action> action, const State& state)
	{
		actions.emplace(actions.begin(),Node(std::move(action), state));
	}

	State Plan::Final() const
	{
		if (actions.empty())
			return start;
		else
			return actions.back().result;
	}

	std::map<unsigned, std::function<Action*(void)>> Action::keymap = 
	{
		{ VK_UP, [](){ return new North(); } },
		{ VK_RIGHT, [](){ return new East(); } },
		{ VK_DOWN, [](){ return new South(); } },
		{ VK_LEFT, [](){ return new West(); } },
	};
} // ::Game