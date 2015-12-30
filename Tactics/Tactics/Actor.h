#pragma once

#include "Position.h"

namespace Game
{
	class State;
	class Actor;

    class Object
    {
    public:
        virtual ~Object() = default;
        virtual void Render() const = 0;
        void Move(int dx, int dy);
		Position GetPosition() const;
    protected:
        Position position;
    };

    class Actor : public Object
    {
    public:
        Actor();
		void Render() const override;
		unsigned GetActionPoints() const;
		void Apply(const State& result);
		void Turn();
    private:
        friend std::wistream& operator>>(std::wistream& s, Actor& actor);
		unsigned actionPoints;
		unsigned team;
	};
    std::wistream& operator>>(std::wistream& s, Actor& actor);

}   // ::Game

