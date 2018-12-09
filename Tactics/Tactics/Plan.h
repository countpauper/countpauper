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
    bool Valid() const;
    void Execute(Game& game) const;
    virtual std::wstring Description() const = 0;
private:
    struct Node
    {
        Node(IGame& state);
        Node(Node& previous, std::unique_ptr<GameState>&& state, std::unique_ptr<Action>&& action);
        Node(const Node&) = delete;
        ~Node();
        Node& operator=(const Node&) = delete;
        bool IsRoot() const;
        bool DeadEnd() const;
        void Render() const;
        GameChances AllOutcomes() const;

         //int Score(const Position& target, unsigned startMovePoints) const;
        bool Compare(const Node& other, const Position& target) const;
        bool operator==(const Node& other) const;
        bool Reached(const Position& target) const;

        Node* previous;
        std::unique_ptr<Action> action;
        double chance;
        std::unique_ptr<GameState> state;
        std::vector<std::unique_ptr<Node>> children;
    };
    struct NodeCompare
    {
        NodeCompare(const Position& target);
        bool operator() (const std::unique_ptr<Node>& a, const std::unique_ptr<Node>& b) const;
        Position target;
    };
    class OpenTree : public std::set < std::unique_ptr<Node>, NodeCompare >
    {
    public:
        OpenTree(const Position& target);
        std::unique_ptr<Node> Pop();
    };
    class ClosedList : public std::set<std::unique_ptr<Node>, NodeCompare>
    {
    public:
        ClosedList(const Position& target);
        bool Contains(const GameState& state) const;
        std::unique_ptr<Node> Extract(Node* node);
    };
    friend ClosedList;
protected:
    void Approach(const Position& target, Game& game, std::unique_ptr<Action>&& action);
    Actor& actor;
    std::unique_ptr<Node> m_root;
private:
    GameChances AllOutcomesRecursive(Node& node) const;
    GameChances AllOutcomes() const;

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