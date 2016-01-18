#pragma once

#include "Position.h"
#include "Direction.h"

namespace Game
{
    class State;
    class Actor;

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
    protected:
        Position position;
        int hp;
        unsigned maxhp;
    };

    class Skill
    {
    public:
        Skill();
        std::unique_ptr<Action> Action() const;

    private:
        friend std::wistream& operator>>(std::wistream& s, Actor& actor);
        std::string m_name;
        unsigned cost;
        float range;
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
            Skill* skill;
            unsigned score;
        };
    private:
        friend std::wistream& operator>>(std::wistream& s, Actor& actor);
        unsigned mp;
        unsigned maxmp;
        unsigned team;
        Direction direction;
    };
    std::wistream& operator>>(std::wistream& s, Actor& actor);

}   // ::Game

