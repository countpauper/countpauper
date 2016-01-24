#pragma once

namespace Game
{
    class Actor;
    class State;

    class IGame
    {
    public:
        virtual State Get(const Actor& actor) const = 0;
        virtual void Apply() = 0;
        virtual void Adjust(Actor& actor, const State& state) = 0;
    };
}
