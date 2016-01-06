#pragma once

#include <vector>
#include <list>
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
    };
    struct Branch : public Node
    {
        Branch(const State& state, const Position& target);
        Branch(Branch& previous, std::unique_ptr<Action> action, const State& state, const Position& target);
        //int Score(const Position& target, unsigned startMovePoints) const;
        bool operator<(const Branch& other) const;
        bool Reached() const;
    public:
        Branch* previous;
        Position target;
    };
    typedef std::list<std::unique_ptr<Branch>> OpenTree;
    class ClosedList : public std::list<std::unique_ptr<Branch>>
    {
    public:
        ClosedList() = default;
        bool Contains(const State& state) const;
        Plan::Branch *Best(const Position& target, unsigned startMovePoints) const;
    };
    friend ClosedList;
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