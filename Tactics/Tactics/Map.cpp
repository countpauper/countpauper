#include "stdafx.h"
#include "Map.h"
#include "Color.h"
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
		return true;
	}

	void Map::Render() const
    {
        RGBA colorTable[] = {
            {   0,   0,   0, 255 }, // None = 0
            {   0, 255,   0, 255 }, // Nature,
            {  64,  64,  64, 255 }, // Earth
            { 255, 255, 255,  24 }, // Air
            {   0,   0, 255, 128 }, // Water
            { 255,   0,   0, 255 }, //  Fire
        };

        glBegin(GL_TRIANGLES);
        unsigned i = 0;
        for (float y = 0; y < height; ++y)
        {
			for (float x = 0; x < width; ++x)
			{
				const auto& square = squares.at(i++);
				auto color = colorTable[unsigned(square.floor)];
				color.Render();
				auto z = square.Z();
				glVertex3f(x,		z, y);
				glVertex3f(x + 1,	z, y);
				glVertex3f(x + 1,	z, y + 1);

				glVertex3f(x,		z, y);
				glVertex3f(x + 1,	z, y + 1);
				glVertex3f(x,		z, y + 1);
				
				auto wallx = square.walls[0];
				if (wallx != Wall::None)
				{
					float zn = 0;
					auto neighbour = MaybeAt(Position(x + 1, y));
					if (neighbour)
					{
						zn = neighbour->Z();
					}
					auto wallColor = colorTable[unsigned(wallx)];
					wallColor.Render();
					glVertex3f(x + 1,   z, y);
					glVertex3f(x + 1, zn, y);
					glVertex3f(x + 1, z, y + 1);
					
					glVertex3f(x + 1,	zn,	y);
					glVertex3f(x + 1,	zn,	y + 1);
					glVertex3f(x + 1,	z,	y + 1);
				}

				auto wally = square.walls[1];
				if (wally != Wall::None)
				{
					float zn = 0;
					auto neighbour = MaybeAt(Position(x, y-1));
					if (neighbour)
					{
						zn = neighbour->Z();
					}
					auto wallColor = colorTable[unsigned(wally)];
					wallColor.Render();
					glVertex3f(x,		z,	y);
					glVertex3f(x,		zn,	y);	
					glVertex3f(x + 1,	z,	y);

					glVertex3f(x,		zn,	y);
					glVertex3f(x + 1,	zn,	y);
					glVertex3f(x + 1,	z,	y);
				}

			}
        }
        glEnd();

        glColor3f(0, 0, 0);
        glBegin(GL_LINES);
		i = 0;
        for (float y = 0; y < height; ++y)
        {
            for (float x = 0; x < width; ++x)
            {
				const auto& square = squares.at(i++);
				auto z = square.Z();
				glVertex3f(x,	z, y);
                glVertex3f(x+1, z, y);

                glVertex3f(x+1, z, y);
                glVertex3f(x+1, z, y+1);

                glVertex3f(x+1, z, y+1);
                glVertex3f(x,	z, y+1);

                glVertex3f(x, z, y+1);
                glVertex3f(x, z, y);
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
        unsigned short floor, wall0, wall1;
        s >> floor >> wall0 >> wall1;
        square.floor = Floor(floor);
        square.walls[0] = Wall(wall0);
        square.walls[1] = Wall(wall1);
		s >> square.height;
        return s;
    }


}   // ::Game


