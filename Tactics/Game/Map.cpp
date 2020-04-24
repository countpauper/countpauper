#include "stdafx.h"
#include <gl/GL.h>
#include "Engine/Image.h"
#include "Engine/Error.h"
#include "Map.h"

namespace Game
{

    float Square::Z() const
    {
        return float(height)*0.25f;
    }

    FlatMap::FlatMap() :
        width(0),
        height(0)
    {

    }
    FlatMap::~FlatMap() = default;

    Square FlatMap::At(const Position& p) const
    {
        if (p.x < 0)
            return Square();
        if (p.y < 0)
            return Square();
        if (unsigned(p.x) >= Longitude())
            return Square();
        if (unsigned(p.y) >= Latitude())
            return Square();
         const auto& square = squares.at(p.x + p.y*width);
         if (p.z != square.height)
             return Square();
         return square;
    }

    bool Map::CanBe(const Position& position) const
    {
        if ((position.x < 0 || position.y < 0) ||
            (position.x >= int(Latitude())) || (position.y >= int(Longitude())))
            return false;
        auto square = At(position);
        if (!square.solid)
            return false;   
        return true;
    }

    bool Map::CanGo(const Position& from, Direction direction) const
    {
        if (direction.IsNone())
            return false;   // going nowhere is not going 
        auto square = At(from);
        auto neighbour = At(from + direction.Vector());
        if (!neighbour)
            return false;
        if (std::abs(int(square.height) - int(neighbour.height)) > 1)
            return false;
        return true;
    }

    unsigned FlatMap::Latitude() const
    {
        return width;
    }
    unsigned FlatMap::Longitude() const
    {
        return height;
    }
    std::vector<Engine::RGBA> Square::colorTable = {
        { 0, 0, 0, 255 }, // None = 0
        { 0, 255, 0, 255 }, // Nature,
        { 64, 64, 64, 255 }, // Earth
        { 255, 255, 255, 24 }, // Air
        { 0, 0, 255, 128 }, // Water
        { 255, 0, 0, 255 }, //  Fire
    };

    void Square::RenderFloor(int x, int y, int w, int h) const
    {
        //auto color = colorTable[unsigned(floor)];
        //color.Render();
        glColor3f(1.0f, 1.0f, 1.0f);
        auto z = Z();
        glNormal3f(0.0f, 1.0f, 0.0f);
        float dx = 1.0f / float(w);
        float dy = 1.0f / float(h);
        glBegin(GL_QUADS);
            glTexCoord2f(float(x)*dx, float(y)*dy);
            glVertex3f(0.0f, z, 0.0f);

            glTexCoord2f(float(x+1)*dx, float(y)*dy);
            glVertex3f(1.0f, z, 0.0f);

            glTexCoord2f(float(x+1)*dx, float(y+1)*dy);
            glVertex3f(1.0f, z, 1.0f);

            glTexCoord2f(float(x)*dx, float(y+1)*dy);
            glVertex3f(0.0f, z, 1.0f);
        glEnd();
    }

    void Square::RenderXWall(const Square& neighbour) const
    {
        auto wallx = walls[0];
        if (wallx == Element::None)
            return;
        float z = Z();
        float zn = 0;
        if (neighbour)
        {
            zn = neighbour.Z();
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

    void Square::RenderYWall(const Square& neighbour) const
    {
        auto wally = walls[1];
        if (wally == Element::None)
            return;

        float z = Z();
        float zn = 0;
        if (neighbour)
        {
            zn = neighbour.Z();
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

    void FlatMap::Tick(double seconds)
    {

    }
    void FlatMap::Render() const
    {
        Engine::Image::Bind bind(texture);
        unsigned i = 0;
        for (unsigned y = 0; y < height; ++y)
        {
            for (unsigned x = 0; x < width; ++x)
            {
                const auto& square = squares.at(i++);
                glPushMatrix();
                glPushName(LocationName(Position(x,y,square.height), Direction::none));
                glTranslatef(float(x), 0.0f, float(y));
                    square.RenderFloor(x,y,width,height);
                    square.RenderXWall(At(Position(x + 1, y, 0)));
                    square.RenderYWall(At(Position(x, y-1, 0)));
                    square.RenderOutline();
                glPopName();
                glPopMatrix();
            }
        }
        Engine::CheckGLError();
    }

    Engine::Coordinate Map::Coordinate(const Position& p) const
    {
        return Engine::Coordinate(static_cast<float>(p.x), static_cast<float>(p.y), static_cast<float>(At(p).height));
    }

    uint32_t Map::LocationName(const Position& p, const Direction& dir)
    {
        if (p.x >= 1 << 9)
            throw std::range_error("X-Coordinate too large to encode in 9 bits");
        if (p.y>= 1<<9)
            throw std::range_error("Y-Coordinate too large to encode in 9 bits");
        if (p.z >= 1 << 10)   // more bits for z, because vertical El < horizontal El
            throw std::range_error("Z-Coordinate too large to encode in 10 bits");
        auto id = dir.Id();
        if (id>=1<<4)
            throw std::range_error("Direction too large to encode in 4 bits");
        return (id<<28) + (p.z << 18) + (p.y << 9) + p.x;
    }
    
    Position Map::NamedLocation(uint32_t name)
    {
        return Position(name & 0x1FF, (name>> 9) & 0x1FF, (name>>18) & 0x3FF);
    }

    std::wistream& operator>>(std::wistream& s, FlatMap& map)
    {
        s >> map.name >> map.width >> map.height;
        std::wstring textureName;
        s >> textureName;
        if (textureName!=L"Null")    
            map.texture.Load(textureName);
        map.squares.resize(map.width * map.height);
        for (auto& square : map.squares)
            s >> square;
        return s;
    }

    std::wistream& operator>>(std::wistream& s, Square& square)
    {
        unsigned short floor, wall0, wall1;
        s >> floor >> wall0 >> wall1;
        square.floor = Element(floor);
        square.walls[0] = Element(wall0);
        square.walls[1] = Element(wall1);
        s >> square.height;
        square.solid = square.floor == Element::Stone || 
                        square.floor == Element::Nature;
        return s;
    }


}   // ::Game


