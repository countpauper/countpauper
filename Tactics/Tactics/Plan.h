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
    Plan(Actor& actor);
    void Render() const;
    void Add(std::unique_ptr<Action> action, const State& state);
    State Final() const;
    void Execute(Actor& actor) const;
private:
	Actor& actor;
	struct Node
	{
		Node(const State& state);
		Node(std::unique_ptr<Action> action, const State& state);
		Node(const Node&) = delete;
		Node(Node&& other);
		Node& operator= (Node&&);

		std::unique_ptr<Action> action;
		State result;
		int Score(const Position& target, unsigned startMovePoints) const;
		bool Reached(const Position& target) const;
	};
	struct Link : public Node
	{
		Link(const State& state);
		Link(Link& previous, std::unique_ptr<Action> action, const State& state);
		Link* previous;
	};
protected:
	void Approach(const Position& target, const Game& game);
	void AddFront(Node& node);
	std::vector<Node> actions;
};

class PathPlan : public Plan
{
public:
    PathPlan(Actor& actor, const Position& target, const Game& game);
private:
    Position target;
};

class AttackPlan : public Plan
{
public:
	AttackPlan(Actor& actor, const Actor& target, const Game& game);    // TODO: action factory 
private:
    Actor& target;
};


}   // ::Game