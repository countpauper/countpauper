#pragma once

#include <memory>
#include <vector>
#include "Engine/Random.h"
#include "Game/Position.h"
#include "Game/Direction.h"
#include "Game/Body.h"
#include "Game/Stats.h"
#include "Game/Score.h"
#include "Item.h"
#include "Skills.h"
#include "Target.h"

namespace Game
{
    class State;
    class Action;
    class Plan;

    class Object : public Target
    {
    public:
        Object();
        virtual ~Object() = default;
        virtual void Activate(const Game& game) = 0;
        virtual void Render() const = 0;
        unsigned Id() const;
        virtual bool Prone() const;
        void Move(int dx, int dy);
        Position GetPosition() const override;
        std::wstring Description() const override;
        Body body;    // TODO: different body parts for different objects/creatures
    protected:
        Position position;
        std::wstring name;
    };
    class Actor : public Object
    {
    public:
        Actor();
        void Render() const override;
        unsigned GetMovePoints() const;
        Direction GetDirection() const;
        void Apply(const State& result);
        bool IsAlly(const Actor& other) const;
        void Activate(const Game& game) override;
        bool Trigger(const Actor& actor, Game& game);
        bool IsActive() const;
        bool IsIdle() const;    // active, but no plan
        bool IsEngaged() const; // has an engaging plan
        bool IsAnticipating() const; 
        
        void AI(Game& game);
        void RestoreRandom();
        void Execute(Game& game);
        unsigned GetTeam() const;
        bool CanAct() const;
        bool Dead() const;
        bool Prone() const override;
        struct Know
        {
            Know() : skill(nullptr), score(0) {}
            const Skill* skill;
            unsigned score;
        };
        using Knowledge = std::vector<Know> ;
        const Skill* DefaultAttack() const;
        const Skill* DefaultMove() const;
        const Skill* WaitSkill() const;
        std::vector<std::unique_ptr<Action>> AllMoves(const Position& from) const;

        std::vector<const Skill*> FollowSkill(const Skill& previous, Skill::Trigger trigger) const;

        bool IsPossible(const Skill& skill) const;
        const Knowledge& GetSkills() const;
        unsigned GetSkillScore(const Skill& skill) const;
        int MovePoints() const;

        std::vector<const Armor*> Worn() const;
        std::vector<const Weapon*> Wielded() const;
        std::vector<Know> GetKnowledge() const;
        std::unique_ptr<Plan> plan;
    private:
        Bonus AgilityMoveBonus() const;
        Score GetMaxMovePoints() const;

        friend std::wistream& operator>>(std::wistream& s, Actor& actor);
        bool active;
        Engine::Generator::Seed randomState;
        unsigned mp;
        unsigned team;
        Direction direction;
        Knowledge knowledge;
        std::vector<Armor> worn;
        std::vector<Weapon> wielded;
    };
    std::wistream& operator>>(std::wistream& s, Actor& actor);

}   // ::Game

