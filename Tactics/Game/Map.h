#pragma once
#include <vector>
#include <array>
#include <memory>
#include "Engine/Image.h"
#include "Engine/Color.h"
#include "Engine/Coordinate.h"
#include "Game/Position.h"
#include "Physics/Direction.h"
#include "Physics/Box.h"
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
        virtual void Render() const = 0;
        virtual void Tick(double seconds) = 0;
        virtual Element Get(const Position& p) const = 0;
        virtual Physics::Box Bounds() const =0;

        unsigned Latitude() const; // Breath, x
        unsigned Longitude() const; // Length, y 

        bool CanBe(const Position& position) const;
        // TODO: use force of streams and gravity Physics::Direction AtRest(const Position& position, bool aerial=false) const;
        Square At(const Position& p) const;
        // TODO: bool CanBreath(const Position& p, bool gills) const; // if air or water at height
        // TODO: pain Hurts(const Position &p)  fire damage or force damage at very high stream power or pressure or in solid
        bool CanGo(const Position& from, Physics::Direction direction) const;
        Engine::Coordinate Coordinate(const Position& p) const;
        static Position NamedLocation(uint32_t name);
        static uint32_t LocationName(const Position& p, const Physics::Direction& dir);
    };

    class FlatMap : public Map
    {
    public:
        FlatMap();
        ~FlatMap();
        void Render() const override;
        void Tick(double seconds) override;
    protected:
        Physics::Box Bounds() const override;
        Element Get(const Position& p) const override;
    private:
        friend std::wistream& operator>>(std::wistream& s, FlatMap& map);
        std::wstring name;
        unsigned width;
        unsigned height;
        unsigned short highest_square;
        std::vector<Square> squares;
        Engine::Image texture;
    };
    std::wistream& operator>>(std::wistream& s, FlatMap& map);
    std::wistream& operator>>(std::wistream& s, Square& square);

}   // ::Game



