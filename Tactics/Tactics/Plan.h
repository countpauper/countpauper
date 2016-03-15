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
class Skill;

class Plan
{
public:
    Plan(Actor& actor);
    virtual ~Plan() = default;
    void Render() const;
    void Add(Game& game, std::unique_ptr<Action> action, const Outcomes& outcomes);
    void Add(std::unique_ptr<Action> action, const State& state);
    State Final() const;
    void Execute(Game& game) const;
    virtual std::wstring Description() const = 0;
protected:
    Actor& actor;
private:
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
    void Approach(const Position& target, Game& game, std::unique_ptr<Action>&& action);
    void AddFront(Node& node);
    void AddAlternatives(Game& game, const Outcomes& outcomes);

    std::vector<Node> actions;
    std::vector<GameChance> result;
};

class PathPlan : public Plan
{
public:
    PathPlan(Actor& actor, const Position& target, Game& game);
    std::wstring Description() const override;
private:
    Position target;
};

class AttackPlan : public Plan
{
public:
    AttackPlan(Actor& actor, Actor& target, Game& game, const Skill& skill);
    const Skill& skill;
    std::wstring Description() const override;
private:
    Actor& target;
};

class ManualPlan : public Plan
{
public:
    ManualPlan(Actor& actor);
    std::wstring Description() const override;
};

}   // ::Game