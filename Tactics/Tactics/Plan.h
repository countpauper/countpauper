#pragma once

#include <vector>
#include <memory>
#include <functional>
#include "Direction.h"
#include "State.h"
#include "Action.h"

namespace Game
{

class Plan
{
public:
	Plan(const Actor& actor);
	void Render() const;
	void Add(std::unique_ptr<Action> action, const State& state);
	void AddFront(std::unique_ptr<Action> action, const State& state);
	State Final() const;
	void Execute(Actor& actor) const;
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