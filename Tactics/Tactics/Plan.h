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
    void Add(IGame& game, std::unique_ptr<Action> action);
    State Final() const;
    bool Valid() const;
    void Execute(Game& game) const;
    virtual std::wstring Description() const = 0;
private:
    struct Node
    {
        Node(IGame& state);
        Node(std::shared_ptr<Node> previous, IGame& state, std::unique_ptr<Action>&& action);
        Node(const Node&) = delete;
        ~Node();
        Node& operator=(const Node&) = delete;
        bool DeadEnd() const;
        const GameState& ExpectedState() const;
        GameState& ExpectedState();
        //int Score(const Position& target, unsigned startMovePoints) const;
        bool Compare(const Node& other, const Position& target) const;
        bool operator==(const Node& other) const;
        bool Reached(const Position& target) const;

        IGame& state;
        std::unique_ptr<Action> action;
        GameChances result;
        std::shared_ptr<Node> previous;
    };
    struct NodeCompare
    {
        NodeCompare(const Position& target);
        bool operator() (const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) const;
        Position target;
    };
    class OpenTree : public std::set < std::shared_ptr<Node>, NodeCompare >
    {
    public:
        OpenTree(const Position& target);
    };
    class ClosedList : public std::set<std::shared_ptr<Node>, NodeCompare>
    {
    public:
        ClosedList(const Position& target);
        bool Contains(const GameState& state) const;
    };
    friend ClosedList;
protected:
    void Approach(const Position& target, Game& game, std::unique_ptr<Action>&& action);
    Actor& actor;
    std::shared_ptr<Node> m_actions;
private:
    using Outcomes = std::map < double, GameChance* > ;
    Outcomes AllOutcomesRecursive(Node& node) const;
    Outcomes AllOutcomes() const;

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