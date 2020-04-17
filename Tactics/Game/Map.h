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
        virtual ~Map()=default;
        virtual Square At(const Position& p) const=0;
        virtual const Square* MaybeAt(const Position& p) const=0;
        virtual void Render() const = 0;
        virtual bool CanBe(const Position& position) const = 0;
        virtual bool CanGo(const Position& from, Direction direction) const=0;

        Engine::Coordinate Coordinate(const Position& p) const;
    };

    class FlatMap : public Map
    {
    public:
        FlatMap();
        ~FlatMap();
        Square At(const Position& p) const override;
        const Square* MaybeAt(const Position& p) const override;
        void Render() const override;
        bool CanBe(const Position& position) const override;
        bool CanGo(const Position& from, Direction direction) const override;
    private:
        friend std::wistream& operator>>(std::wistream& s, FlatMap& map);
        std::wstring name;
        unsigned width;
        unsigned height;
        std::vector<Square> squares;
        Engine::Image texture;
    };
    std::wistream& operator>>(std::wistream& s, Map& map);
    std::wistream& operator>>(std::wistream& s, Square& square);

}   // ::Game



