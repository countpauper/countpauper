#pragma once
#include <vector>
#include <array>
#include <memory>
#include "Engine/Image.h"
#include "Engine/Color.h"
#include "Engine/Coordinate.h"
#include "Position.h"
#include "Direction.h"

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
        unsigned short height;
        unsigned short reserved;
        float Z() const;
        void RenderFloor(int x, int y, int w, int h) const;
        void RenderOutline() const;
        void RenderXWall(const Square* neighbour) const;
        void RenderYWall(const Square* neighbour) const;
        static std::vector<Engine::RGBA> colorTable;
    };


    class Map
    {
    public:
        Map();
        ~Map();
        Square At(const Position& p) const;
        const Square* MaybeAt(const Position& p) const;
        void Render() const;
        bool CanBe(const Position& position) const;
        bool CanGo(const Position& from, Direction direction) const;

        Engine::Coordinate Coordinate(const Position& p) const;
    private:
        friend std::wistream& operator>>(std::wistream& s, Map& map);
        std::wstring name;
        unsigned width;
        unsigned height;
        std::vector<Square> squares;
        Engine::Image texture;
    };


    std::wistream& operator>>(std::wistream& s, Map& map);
    std::wistream& operator>>(std::wistream& s, Square& square);

}   // ::Game



