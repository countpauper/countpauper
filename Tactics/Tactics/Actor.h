#pragma once

#include <memory>
#include <vector>
#include "Position.h"
#include "Direction.h"
#include "Body.h"
#include "Stats.h"
#include "Item.h"
#include "Score.h"

namespace Game
{
    class State;
    class Skill;
    class Action;

    class Object
    {
    public:
        Object();
        virtual ~Object() = default;
        virtual void Turn() = 0;
        virtual void Render() const = 0;
        void Move(int dx, int dy);
        Position GetPosition() const;
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
        struct Skill
        {
            Skill() : skill(nullptr), score(0) {}
            const ::Game::Skill* skill;
            unsigned score;
        };
        typedef std::vector<Actor::Skill> Skills;
        const ::Game::Skill* DefaultAttack() const;
        bool IsPossible(const ::Game::Skill& skill) const;
        const Actor::Skills& GetSkills() const;
        unsigned GetSkillScore(const ::Game::Skill& skill) const;
        int MovePoints() const;

        std::vector<const Armor*> Worn() const;
        std::vector<const Weapon*> Wielded() const;
    private:
        Bonus AgilityMoveBonus() const;
        Score GetMaxMovePoints() const;

        friend std::wistream& operator>>(std::wistream& s, Actor& actor);
        unsigned mp;
        unsigned team;
        Direction direction;
        std::vector<Actor::Skill> skills;
        std::vector<Armor> worn;
        std::vector<Weapon> wielded;
    };
    std::wistream& operator>>(std::wistream& s, Actor& actor);

}   // ::Game

