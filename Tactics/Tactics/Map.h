#pragma once
#include <vector>
#include "Position.h"

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

    struct Square
    {
        Floor floor;
        Wall walls[2];
        unsigned short height[4];
        unsigned short reserved;
    };


    class Map
    {
    public:
        Map();
        Square At(const Position& p) const;
        void Render() const;
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



