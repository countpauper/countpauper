#pragma once

namespace Game
{
    constexpr int HorizontalEl = 3; // 1 square is 3x3l
    constexpr int VerticalEl = 1; // 1 block is 3x3x1 l
    constexpr double MeterPerEl = 1.0 / 3.0;
    constexpr double LiterPerVoxel = (HorizontalEl * MeterPerEl) * (HorizontalEl * MeterPerEl)* (VerticalEl * MeterPerEl) * 1000.0;

    struct Position 
    {
        Position();
        Position(int x, int y, int z);
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
    };

	bool operator==(const Position& a, const Position& b);
    bool operator!=(const Position& a, const Position& b);
    Position operator+(const Position& a, const Position& b);
    Position operator-(const Position& a, const Position& b);
    std::ostream& operator<<(std::ostream& stream, const Position& position);
    std::wostream& operator<<(std::wostream& stream, const Position& position);

    // TODO : move some stuff over and disallow some operators
    struct Size : Position
    {
        Size() = default;

        Size(int x, int y, int z) : Position(x, y, z)
        {
        }
        explicit Size(int s) : Size(s, s, s) {}
        explicit Size(const Position& p) : Position(p)
        {
        }
        int Volume() const { return x * y * z; }
    };

    Size operator+(const Size& a, const Size& b);
    Size operator+(const Size& a, int s);

}   // ::Game

