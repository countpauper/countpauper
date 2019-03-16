#pragma once

namespace Game
{
    class Actor;
    class State;

    class IGame
    {
    public:
        virtual const Actor* Executor() const = 0;
        virtual State Get(const Actor& actor) const = 0;
        virtual void Apply(IGame& root) const = 0;
        virtual void Adjust(const Actor& actor, const State& state, const std::wstring& description) = 0;
        virtual std::wstring Description() const = 0;
        virtual bool CanBe(const Position& position) const = 0;
        virtual bool CanGo(const Position& from, Direction direction) const = 0;
        virtual bool Cover(const Position& from, const Position& to) const = 0;
    };
}
