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
        float Size() const;;
        unsigned ManSize() const;
        Position& operator+=(const Position& delta);
        Position& operator-=(const Position& delta);
        operator bool() const;
        std::wstring Description() const;
    };
    bool operator==(const Position& a, const Position& b);
    bool operator!=(const Position& a, const Position& b);
    Position operator+(const Position& a, const Position& b);
    Position operator-(const Position& a, const Position& b);
}   // ::Game
