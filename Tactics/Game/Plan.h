#pragma once

#include <vector>
#include <list>
#include <memory>
#include <functional>
#include "Physics/Direction.h"
#include "State.h"
#include "Action.h"
#include "Future.h"

namespace Game
{
class Skill;

class Plan
{
public:

    class Condition
    {
    public:
        virtual bool Anticipating(const Target& target) const = 0;
    };
    class Wait : public Condition
    {
    public:
        Wait(const Actor& target);
        bool Anticipating(const Target& target) const override;
    private:
        const Actor& actor;
    };

    Plan(const Actor& actor);
    virtual ~Plan() = default;
    void Render() const;
    bool Anticipating() const;
    bool Trigger(const Target& target);
    bool Valid() const;
    virtual void Compute(const Game& game) = 0;
    virtual bool Execute(Game& game) const;
    virtual std::wstring Description() const = 0;
    Engine::Image Icon() const;

	// Helper functions that are easy/important to unit test 
	static std::map<int, double> CenterMass(double center, double sigma = 1.0, double limit = 0.001);
	static std::map<Location, double> HitLocations(const State& attacker, const Skill& skill, const State& victim, Physics::Direction trajectory);
	using HitChances = std::map<const Part*, double>;
	static std::tuple<HitChances, Physics::Direction> Aim(State attacker, const State& victim, const Skill& skill);
	static Physics::Direction Intercept(const IGame& state, const Identity& actor, const TargetedAction& action, const Skill& skill);
protected:
	using Outcomes = std::vector<std::pair<double, Future*>>;
	
	struct Node
    {
        Node(const IGame& state, const Actor& executor);
		Node(Node& previous, std::unique_ptr<Action>&& action);

        Node(const Node&) = delete;
        ~Node();
        Node& operator=(const Node&) = delete;
        bool IsRoot() const;
        bool DeadEnd() const;
        void Render() const;
        Node* Next() const;
		Outcomes AllOutcomes() const;

         //int Score(const Position& target, unsigned startMovePoints) const;
        bool Compare(const Node& other, const Position& target) const;
        bool operator==(const Node& other) const;
        bool Reached(const Position& target) const;

		Node* previous;
        std::unique_ptr<Action> action;

		double chance;
        std::unique_ptr<Future> state;
        std::vector<std::unique_ptr<Node>> children;
    };
private:
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
        bool Contains(const Future& state) const;
        std::unique_ptr<Plan::Node> ExtractRoot(std::unique_ptr<Node>& leaf);
    private:
        std::unique_ptr<Node> Extract(Node* node);
    };
    friend ClosedList;
protected:
    std::unique_ptr<Node> Approach(const Target& target, const Game& game, const Skill& skill);
    std::unique_ptr<Node> Goto(const Position& target, const Game& game);
    bool PlanAction(Plan::Node& parent, const Skill& skill, const Actor& actor, const Target& target);
    void PlanCombo(Node& parent, const Skill& skill, const Actor& actor, const Target& target);
    Node* PlanReaction(Plan::Node& parent, const Skill& skill, const Actor& defender, const TargetedAction& offense);
    static std::vector<const Skill*> Combo(const Actor& actor, const Skill& previous);
    virtual const Skill* GetSkill() const = 0;
    const Actor& actor;
    std::unique_ptr<Condition> condition;
    std::unique_ptr<Node> root;
private:
	Outcomes AllOutcomesRecursive(Node& node) const;
	Outcomes AllOutcomes() const;
    std::vector<Action*> ActionSequence(const Future& end) const;
    std::vector<std::wstring> Descriptions(const Future& end) const;
    void Apply(const Future& state, Game& game) const;
};

class WaitPlan : public Plan
{
public:
    WaitPlan(const Actor& actor, const Target& target, const Game& game);
    std::wstring Description() const override;
    void Compute(const Game& game) override;
private:
    const Skill* GetSkill() const override;
    const Target& target;
};

class SkipPlan : public Plan
{
public:
    SkipPlan(const Actor& actor, const Game& game);
    std::wstring Description() const override;
    void Compute(const Game& game) override;
    bool Execute(Game& game) const override;
private:
    const Skill* GetSkill() const override;
};

class PathPlan : public Plan
{
public:
    PathPlan(const Actor& actor, const Position& target, const Game& game);
    std::wstring Description() const override;
    void Compute(const Game& game) override;
private:
    const Skill* GetSkill() const override;
    Position target;
};

class AttackPlan : public Plan
{
public:
    AttackPlan(const Actor& actor, const Target& target, const Game& game, const Skill& skill);
    const Skill& skill;
    std::wstring Description() const override;
    void Compute(const Game& game) override;
private:
    const Skill* GetSkill() const override;
    const Target& target;
};

class ManualPlan : public Plan
{
public:
    ManualPlan(Actor& actor);
    std::wstring Description() const override;
    void Compute(const Game& game) override;
private:
    const Skill* GetSkill() const override;
};

}   // ::Game