#include "stdafx.h"
#include "Map.h"
#include <gl/GL.h>

namespace Game
{

    float Square::Z() const
    {
        return static_cast<float>(height)*0.25f;
    }

    Map::Map() :
        width(0),
        height(0)
    {

    }
    Square Map::At(const Position& p) const
    {
        assert(p.x >= 0);
        assert(p.y >= 0);
        assert(p.x < int(width));
        assert(p.y < int(height));
        return squares.at(p.x + p.y*width);
    }

    const Square* Map::MaybeAt(const Position& p) const
    {
        if (p.x < 0)
            return nullptr;
        if (p.y < 0)
            return nullptr;
        if (p.x >= int(width))
            return nullptr;
        if (p.y >= int(height))
            return nullptr;
        return &squares.at(p.x + p.y*width);
    }

    bool Map::CanBe(const Position& position) const
    {
        if ((position.x < 0 || position.y < 0) ||
            (position.x >= int(width)) || (position.y >= int(height)))
            return false;
        auto square = At(position);
        if ((square.floor == Floor::Air) || (square.floor == Floor::Water))
            return false;
        return true;
    }

    bool Map::CanGo(const Position& from, Direction direction) const
    {
        auto square = At(from);
        auto neighbour = MaybeAt(from + direction.Vector());
        if (!neighbour)
            return false;
        if (std::abs(int(square.height) - int(neighbour->height)) > 1)
            return false;
        return true;
    }

    std::vector<RGBA> Square::colorTable = {
        { 0, 0, 0, 255 }, // None = 0
        { 0, 255, 0, 255 }, // Nature,
        { 64, 64, 64, 255 }, // Earth
        { 255, 255, 255, 24 }, // Air
        { 0, 0, 255, 128 }, // Water
        { 255, 0, 0, 255 }, //  Fire
    };

    void Square::RenderFloor() const
    {
        auto color = colorTable[unsigned(floor)];
        color.Render();
        auto z = Z();
        glNormal3f(0.0f, 1.0f, 0.0f);
        glBegin(GL_QUADS);
            glVertex3f(0.0f, z, 0.0f);
            glVertex3f(1.0f, z, 0.0f);
            glVertex3f(1.0f, z, 1.0f);
            glVertex3f(0.0f, z, 1.0f);
        glEnd();
    }

    void Square::RenderXWall(const Square* neighbour) const
    {
        auto wallx = walls[0];
        if (wallx != Wall::None)
        {
            float z = Z();
            float zn = 0;
            if (neighbour)
            {
                zn = neighbour->Z();
            }
            auto wallColor = colorTable[unsigned(wallx)];
            wallColor.Render();
            glNormal3f(1.0f, 0.0f, 0.0f);
            glBegin(GL_QUADS);
                glVertex3f(1.0f, z, 0.0f);
                glVertex3f(1.0f, zn, 0.0f);
                glVertex3f(1.0f, zn, 1.0f);
                glVertex3f(1.0f, z, 1.0f);
            glEnd();
        }
    }

    void Square::RenderYWall(const Square* neighbour) const
    {
        auto wally = walls[1];
        if (wally != Wall::None)
        {
            float z = Z();
            float zn = 0;
            if (neighbour)
            {
                zn = neighbour->Z();
            }
            auto wallColor = colorTable[unsigned(wally)];
            wallColor.Render();
            glNormal3f(0.0f, 0.0f, 1.0f);
            glBegin(GL_QUADS);
                glVertex3f(0.0f, z, 0.0f);
                glVertex3f(0.0f, zn, 0.0f);
                glVertex3f(1.0f, zn, 0.0f);
                glVertex3f(1.0f, z, 0.0f);
            glEnd();
        }
    }

    void Square::RenderOutline() const
    {
        auto z = Z();
        glColor3f(0, 0, 0);
        glBegin(GL_LINES);
            glVertex3f(0.0f, z, 0.0f);
            glVertex3f(1.0f, z, 0.0f);

            glVertex3f(1.0f, z, 0.0f);
            glVertex3f(1.0f, z, 1.0f);

            glVertex3f(1.0f, z, 1.0f);
            glVertex3f(0.0f, z, 1.0f);

            glVertex3f(0.0f, z, 1.0f);
            glVertex3f(0.0f, z, 0.0f);
        glEnd();

    }
    void Map::Render() const
    {
        unsigned i = 0;
        for (unsigned y = 0; y < height; ++y)
        {
            for (unsigned x = 0; x < width; ++x)
            {
                glPushMatrix();
                glPushName((y << 16) + x);
                glTranslatef(static_cast<float>(x), 0.0f, static_cast<float>(y));
                const auto& square = squares.at(i++);
                square.RenderFloor();
                square.RenderXWall(MaybeAt(Position(x + 1, y)));
                square.RenderYWall(MaybeAt(Position(x, y-1)));
                square.RenderOutline();
                glPopName();
                glPopMatrix();
            }
        }
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
        unsigned short floor, wall0, wall1;
        s >> floor >> wall0 >> wall1;
        square.floor = Floor(floor);
        square.walls[0] = Wall(wall0);
        square.walls[1] = Wall(wall1);
        s >> square.height;
        return s;
    }


}   // ::Game


