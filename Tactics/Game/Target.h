#pragma once

#include <sstream>
#include "Game/Position.h"

namespace Game
{
    class Target
    {
    public:
        virtual Position GetPosition() const = 0;
        virtual std::wstring Description() const = 0;
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
        std::wstring Description() const override
        {
            std::wstringstream s;
            s << position;
            return s.str();
        }
    private:
        Position position;
    };
}