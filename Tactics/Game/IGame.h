#pragma once
#include <string>
#include "Game/Position.h"
#include "Physics/Direction.h"

namespace Game
{
    class Actor;
    class State;
	class Identity;

    class IGame
    {
    public:
        virtual const Identity* Executor() const = 0;
        virtual State Get(const Identity& actor) const = 0;
        virtual void Apply(IGame& root) const = 0;
        virtual void Adjust(const Identity& actor, const State& state, const std::wstring& description) = 0;
        virtual std::wstring Description() const = 0;
        virtual bool CanBe(const Position& position) const = 0;
        virtual bool CanGo(const Position& from, Physics::Direction direction) const = 0;
        virtual bool Cover(const Position& from, const Position& to) const = 0;
    };
}
