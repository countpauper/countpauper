#pragma once

#include "Position.h"

namespace Game
{
    class Object
    {
    public:
        virtual ~Object() = default;
        virtual void Render() const = 0;
        void Move(int dx, int dy);
    protected:
        Position position;
    };

    class Actor : public Object
    {
    public:
        Actor() = default;
        void Render() const override;
    private:
        friend std::wistream& operator>>(std::wistream& s, Game::Actor& actor);
    };
    std::wistream& operator>>(std::wistream& s, Game::Actor& actor);

}   // ::Game

