#pragma once

#include "Position.h"

namespace Game
{
	class Result;
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
		void Apply(const Result& result);
    private:
        friend std::wistream& operator>>(std::wistream& s, Actor& actor);
		unsigned actionPoints;
		unsigned team;
	};
    std::wistream& operator>>(std::wistream& s, Actor& actor);

}   // ::Game

