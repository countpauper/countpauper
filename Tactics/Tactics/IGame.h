#pragma once

namespace Game
{
    class Actor;
    class State;

    class IGame
    {
    public:
        virtual Actor* ActiveActor() const = 0;
        virtual State Get(const Actor& actor) const = 0;
        virtual void Apply() const = 0;
        virtual void Adjust(Actor& actor, const State& state) = 0;
        virtual std::wstring Description() const = 0;
        virtual bool CanGo(const Position& from, Direction direction) const = 0;
        virtual bool Cover(const Position& from, const Position& to) const = 0;
    };
}
