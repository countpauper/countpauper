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
    private:
        friend std::wistream& operator>>(std::wistream& s, Actor& actor);
        unsigned mp;
        unsigned maxmp;
        unsigned team;
        Direction direction;
    };
    std::wistream& operator>>(std::wistream& s, Actor& actor);

}   // ::Game

