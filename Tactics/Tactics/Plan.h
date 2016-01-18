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
        Branch(const State& state);
        Branch(Branch& previous, std::unique_ptr<Action> action, const State& state);
        //int Score(const Position& target, unsigned startMovePoints) const;
        bool Compare(const Branch& other, const Position& target) const;
        bool operator==(const Branch& other) const;
        bool Reached(const Position& target) const;
    public:
        Branch* previous;
    };
    struct BranchCompare
    {
        BranchCompare(const Position& target);
        bool operator() (const std::unique_ptr<Branch>& a, const std::unique_ptr<Branch>& b) const;
        Position target;
    };
    class OpenTree : public std::set < std::unique_ptr<Branch>, BranchCompare >
    {
    public:
        OpenTree(const Position& target);
    };
    class ClosedList : public std::set<std::unique_ptr<Branch>, BranchCompare>
    {
    public:
        ClosedList(const Position& target);
        bool Contains(const State& state) const;
    };
    friend ClosedList;
protected:
    void Approach(const Position& target, const Game& game, std::unique_ptr<Action>&& action);
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
    AttackPlan(Actor& actor, Actor& target, const Game& game);    // TODO: action factory 
private:
    Actor& target;
};


}   // ::Game