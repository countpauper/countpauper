#pragma once

#include "Position.h"

namespace Game
{
    class State;
    class Actor;

    class Object
    {
    public:
        Object();
        virtual ~Object() = default;
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
        void Apply(const State& result);
        void Turn();
    private:
        friend std::wistream& operator>>(std::wistream& s, Actor& actor);
        unsigned mp;
        unsigned maxmp;
        unsigned team;
    };
    std::wistream& operator>>(std::wistream& s, Actor& actor);

}   // ::Game

