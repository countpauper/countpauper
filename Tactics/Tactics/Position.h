#pragma once

namespace Game
{
    struct Position
    {
        Position();
        Position(int x, int y);
        int x;
        int y;
        unsigned ManDistance(const Position& other) const;
        float Distance(const Position& other) const;
        Position& operator+=(const Position& delta);
        Position& operator-=(const Position& delta);
    };
    bool operator==(const Position& a, const Position& b);
    bool operator!=(const Position& a, const Position& b);
    Position operator+(const Position& a, const Position& b);
    Position operator-(const Position& a, const Position& b);
}   // ::Game

