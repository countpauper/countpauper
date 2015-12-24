#include "stdafx.h"
#include "Map.h"

namespace Game
{


    Position::Position() : x(0), y(0) 
    {
    }
    Position::Position(unsigned x, unsigned y) : x(x), y(y) 
    {
    }


    Map::Map() :
        width(0),
        height(0)
    {

    }
    Square Map::At(const Position& p) const
    {
        assert(p.x < width);
        assert(p.y < height);
        return squares.at(p.x + p.y*width);
    }

    std::wistream& operator>>(std::wistream& s, Map& map)
    {
        s >> map.name >> map.width >> map.height;
        map.squares.resize(map.width * map.height);
        for (auto& square : map.squares)
            s >> square;
        return s;
    }

    std::wistream& operator>>(std::wistream& s, Square& square)
    {
        unsigned short floor, object, wall0, wall1;
        s >> floor >> object;
        square.floor = Floor(floor);
        square.object = Object(object);
        s >> wall0 >> wall1;
        square.walls[0] = Wall(wall0);
        square.walls[1] = Wall(wall1);
        s >> square.height[0] >> square.height[1] >> square.height[2] >> square.height[3];
        return s;
    }


}   // ::Game


