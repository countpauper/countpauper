#include "stdafx.h"
#include "Map.h"
#include <gl/GL.h>

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

    void Map::Render()
    {
        struct RGBA { float r, g, b, a; };
        RGBA colorTable[] = {
            { 0.0f, 0.0f, 0.0f, 1.0f },    //None = 0
            { 0.0f, 1.0f, 0.0f, 1.0f },     //Nature,
            { 0.3f, 0.3f, 0.3f, 1.0f }, //Earth
            { 1.0f, 1.0f, 1.0f, 0.1f }, //Air
            { 0.0f, 0.0f, 1.0f, 0.5f }, //Water
            { 1.0f, 0.0f, 0.0f, 1.0f }, //  Fire
        };

        glBegin(GL_TRIANGLES);
        unsigned i = 0;
        for (float y = 0; y < height; ++y)
        {
            for (float x = 0; x < width; ++x)
            {
                const auto& square = squares.at(i++);
                auto color = colorTable[unsigned(square.floor)];
                glColor4f(color.r, color.g, color.b, color.a);
                glVertex3f(x, 0, y);
                glVertex3f(x+1, 0 ,y+1);
                glVertex3f(x+1, 0, y);

                glVertex3f(x, 0, y);
                glVertex3f(x+1, 0, y+1);
                glVertex3f(x, 0, y+1);
            }
        }
        glEnd();

        glColor3f(0, 0, 0);
        glBegin(GL_LINES);
        for (float y = 0; y < height; ++y)
        {
            for (float x = 0; x < width; ++x)
            {
                glVertex3f(x, 0, y);
                glVertex3f(x+1, 0, y);

                glVertex3f(x+1, 0, y);
                glVertex3f(x+1, 0, y+1);

                glVertex3f(x+1, 0, y+1);
                glVertex3f(x, 0, y+1);

                glVertex3f(x, 0, y+1);
                glVertex3f(x, 0, y);
            }
        }
        glEnd();

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


