#pragma once

#include "Position.h"
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include "Direction.h"

namespace Game
{
	class Actor;
	class Game;

	class State
	{
	public:
		State(const Actor& actor);
		bool possible;
		Position position;
		unsigned actionPoints;
	};

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

	class Plan
	{
	public:
		Plan(const Actor& actor);
		void Render() const;
		void Add(std::unique_ptr<Action> action, const State& state);
		void AddFront(std::unique_ptr<Action> action, const State& state);
		State Final() const;
	private:
		State start;
		struct Node
		{
			Node(std::unique_ptr<Action> action, const State& result);
			Node(const Node&) = delete;
			Node(Node&& other);
			Node& operator= (Node&&);

			std::unique_ptr<Action> action;
			State result;
		};

		std::vector<Node> actions;
	};
}   // ::Game

