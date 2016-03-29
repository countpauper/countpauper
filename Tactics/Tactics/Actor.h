#pragma once

#include <memory>
#include <vector>
#include "Position.h"
#include "Direction.h"
#include "Health.h"
#include "Stats.h"
#include "Item.h"

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
        unsigned GetHitPoints() const;
        std::wstring name;
        Damage damage;
        Stats stats;
    protected:
        Position position;
        unsigned maxhp;
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
        const Actor::Skills& GetSkills() const;
        unsigned GetSkillScore(const ::Game::Skill& skill) const;
        Damage Mitigation() const;
    private:
        friend std::wistream& operator>>(std::wistream& s, Actor& actor);
        unsigned mp;
        unsigned team;
        Direction direction;
        std::vector<Armor> armors;
        std::vector<Weapon> weapons;
        std::vector<Actor::Skill> skills;
    };
    std::wistream& operator>>(std::wistream& s, Actor& actor);

}   // ::Game

