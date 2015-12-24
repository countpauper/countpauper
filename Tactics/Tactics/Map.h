#pragma once
#include <vector>

namespace Game
{
    enum class Floor : unsigned short
    {
        None = 0,
        Nature,
        Earth,
        Air,
        Water,
        Fire
    };
    enum class Wall : unsigned short
    {
        None  =0,
        Nature,
        Earth,
        Air,
        Water,
        Fire
    };

    enum class Object : unsigned short
    {
        None = 0,
        Spawn = 1,
    };
    struct Square
    {
        Floor floor;
        Wall walls[2];
        unsigned short height[4];
        Object object;
    };

    struct Position
    {
        Position();
        Position(unsigned x, unsigned y);
        unsigned x;
        unsigned y;
    };

    class Map
    {
    public:
        Map();
        Square At(const Position& p) const;
    private:
        friend std::wistream& operator>>(std::wistream& s, Map& map);
        std::wstring name;
        unsigned width;
        unsigned height;
        std::vector<Square> squares;
    };

    std::wistream& operator>>(std::wistream& s, Game::Map& map);
    std::wistream& operator>>(std::wistream& s, Game::Square& square);

}   // ::Game



