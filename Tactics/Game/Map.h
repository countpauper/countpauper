#pragma once
#include <vector>
#include <array>
#include <memory>
#include "Engine/Image.h"
#include "Engine/Color.h"
#include "Engine/Coordinate.h"
#include "Position.h"
#include "Direction.h"
#include "Element.h"

namespace Game
{
    struct Square
    {
        Square() :
            floor( Element::None),
            solid(false),
            walls{ Element::None, Element::None },
            height(0)
        {
        }

        Square(Element e, int h = 0) :
            Square(e, e == Element::Stone || e == Element::Nature, h)
        {
        }

        Square(Element e, bool s, int h = 0) :
            floor(e),
            walls{ e, e },
            solid(s),
            height(h)
        {
        }
        operator bool() const
        {
            return floor != Element::None;
        }
        Element floor;
        Element walls[2];
        unsigned short height;
        unsigned short reserved;
        bool solid;
        float Z() const;
        void RenderFloor(int x, int y, int w, int h) const;
        void RenderOutline() const;
        void RenderXWall(const Square& neighbour) const;
        void RenderYWall(const Square& neighbour) const;
        static std::vector<Engine::RGBA> colorTable;
    };


    class Map
    {
    public:
        virtual ~Map()=default;
        virtual Square At(const Position& p) const=0;
        virtual void Render() const = 0;
        virtual void Tick(double seconds) = 0;
        bool CanBe(const Position& position) const;
        bool CanGo(const Position& from, Direction direction) const;
        Engine::Coordinate Coordinate(const Position& p) const;
        static Position NamedLocation(uint32_t name);
        static uint32_t LocationName(const Position& p, const Direction& dir);
    protected:
        virtual unsigned Longitude() const = 0;
        virtual unsigned Latitude() const = 0;
    };

    class FlatMap : public Map
    {
    public:
        FlatMap();
        ~FlatMap();
        Square At(const Position& p) const override;
        void Render() const override;
        void Tick(double seconds) override;
    protected:
        unsigned Latitude() const override; // Breadth, x
        unsigned Longitude() const override; // Length, y 
    private:
        friend std::wistream& operator>>(std::wistream& s, FlatMap& map);
        std::wstring name;
        unsigned width;
        unsigned height;
        std::vector<Square> squares;
        Engine::Image texture;
    };
    std::wistream& operator>>(std::wistream& s, FlatMap& map);
    std::wistream& operator>>(std::wistream& s, Square& square);

}   // ::Game



