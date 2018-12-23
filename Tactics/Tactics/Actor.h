#pragma once

#include <memory>
#include <vector>
#include "Game/Position.h"
#include "Game/Direction.h"
#include "Body.h"
#include "Stats.h"
#include "Item.h"
#include "Score.h"
#include "Skills.h"
#include "Target.h"

namespace Game
{
    class State;
    class Action;

    class Object : public Target
    {
    public:
        Object();
        virtual ~Object() = default;
        virtual void Turn() = 0;
        virtual void Render() const = 0;
        void Move(int dx, int dy);
        Position GetPosition() const override;
        std::wstring name;
        Body body;    // TODO: different body parts for different objects/creatures
    protected:
        Position position;
    };
    class Actor : public Object
    {
    public:
        Actor();
        void Render() const override;
        unsigned GetMovePoints() const;
        Direction GetDirection() const;
        void Apply(const State& result);
        void Turn() override;
        unsigned GetTeam() const;
        bool CanAct() const;
        bool Dead() const;
        struct Know
        {
            Know() : skill(nullptr), score(0) {}
            const Skill* skill;
            unsigned score;
        };
        using Knowledge = std::vector<Know> ;
        const Skill* DefaultAttack() const;
        std::vector<std::unique_ptr<Action>> AllMoves(const Position& from) const;

        std::vector<const Skill*> FollowSkill(const Skill& previous, Skill::Trigger trigger) const;

        bool IsPossible(const Skill& skill) const;
        const Knowledge& GetSkills() const;
        unsigned GetSkillScore(const Skill& skill) const;
        int MovePoints() const;

        std::vector<const Armor*> Worn() const;
        std::vector<const Weapon*> Wielded() const;
        std::vector<Know> GetKnowledge() const;
    private:
        Bonus AgilityMoveBonus() const;
        Score GetMaxMovePoints() const;

        friend std::wistream& operator>>(std::wistream& s, Actor& actor);
        unsigned mp;
        unsigned team;
        Direction direction;
        Knowledge knowledge;
        std::vector<Armor> worn;
        std::vector<Weapon> wielded;
    };
    std::wistream& operator>>(std::wistream& s, Actor& actor);

}   // ::Game

