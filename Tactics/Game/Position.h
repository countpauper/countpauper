#pragma once

namespace Game
{
    static const int HorizontalEl = 3; // 1 square is 3x3l
    static const int VerticalEl = 1; // 1 block is 3x3x1 l
    static const double MeterPerEl = 1.0/3.0;
    static const double LiterPerBlock = (HorizontalEl * MeterPerEl) * (HorizontalEl * MeterPerEl)* (VerticalEl * MeterPerEl) * 1000.0;

    struct Position 
    {
        Position();
        Position(int x, int y);
        int x;
        int y;
        int z;
        unsigned ManDistance(const Position& other) const;
        float Distance(const Position& other) const;
        unsigned DistanceEl(const Position& other) const;
        float Size() const;
        unsigned SizeEl() const;
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

