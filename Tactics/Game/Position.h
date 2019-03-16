#pragma once

namespace Game
{
    static const int HorizontalEl = 3; // 1 square is 3x3l

    struct Position 
    {
        Position();
        Position(int x, int y);
        int x;
        int y;
        unsigned ManDistance(const Position& other) const;
        float Distance(const Position& other) const;
        unsigned DistanceEl(const Position& other) const;
        float Size() const;
        unsigned SizeEl() const;
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

