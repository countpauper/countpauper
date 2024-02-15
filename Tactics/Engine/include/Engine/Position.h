#pragma once
#include <iostream>

namespace Engine
{
    struct Position 
    {
        Position();
        Position(int x, int y, int z);
        int x;
        int y;
        int z;
        unsigned ManDistance(const Position& other) const;
        float Distance(const Position& other) const;
        float Size() const;
        Position& operator+=(const Position& delta);
        Position& operator-=(const Position& delta);
        operator bool() const;
    };

	bool operator==(const Position& a, const Position& b);
    bool operator!=(const Position& a, const Position& b);
    bool operator<(const Position& a, const Position& b); // for map keys
    Position operator+(const Position& a, const Position& b);
    Position operator-(const Position& a, const Position& b);
    std::ostream& operator<<(std::ostream& stream, const Position& position);
    std::wostream& operator<<(std::wostream& stream, const Position& position);
}   // ::Engine

