#pragma once

#include "Position.h"

namespace Game
{
    class Target
    {
    public:
        virtual Position GetPosition() const = 0;
    };

    class Destination : public Target 
    {
    public: 
        Destination() = default;
        Destination(const Position& position) :
            position(position)
        {
        }

        virtual Position GetPosition() const
        {
            return position;
        }
    private:
        Position position;
    };
}