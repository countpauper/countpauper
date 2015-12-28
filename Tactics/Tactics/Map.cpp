#include "stdafx.h"
#include "Map.h"
#include <gl/GL.h>

namespace Game
{

	std::array<float,4> Square::Z() const
	{
		std::array<float, 4> z = {
			static_cast<float>(height[0])*0.25f,
			static_cast<float>(height[1])*0.25f,
			static_cast<float>(height[2])*0.25f,
			static_cast<float>(height[3])*0.25f,
		};
		return z;
	}

	float Square::MiddleZ() const
	{
		auto z = Z();
		return (z[0] + z[1] + z[2] + z[3]) / 4.0f;
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

	void Map::Render() const
    {
        struct RGBA { float r, g, b, a; };
        RGBA colorTable[] = {
            { 0.0f, 0.0f, 0.0f, 1.0f }, // None = 0
            { 0.0f, 1.0f, 0.0f, 1.0f }, // Nature,
            { 0.3f, 0.3f, 0.3f, 1.0f }, // Earth
            { 1.0f, 1.0f, 1.0f, 0.1f }, // Air
            { 0.0f, 0.0f, 1.0f, 0.5f }, // Water
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
				auto z = square.Z();
				glVertex3f(x,		z[0], y);
				glVertex3f(x + 1,	z[2], y + 1);
				glVertex3f(x + 1,	z[1], y);

				glVertex3f(x,		z[0], y);
				glVertex3f(x + 1,	z[2], y + 1);
				glVertex3f(x,		z[3], y + 1);
				
				auto wallx = square.walls[0];
				if (wallx != Wall::None)
				{
					float zn0 = 0, zn3 = 0;
					auto neighbour = MaybeAt(Position(x + 1, y));
					if (neighbour)
					{
						auto neighbourZ = neighbour->Z();
						zn0 = neighbourZ[0];
						zn3 = neighbourZ[3];
					}
					auto wallColor = colorTable[unsigned(wallx)];
					glColor4f(wallColor.r, wallColor.g, wallColor.b, wallColor.a);
					glVertex3f(x + 1,	z[1],	y);
					glVertex3f(x + 1,	z[2],	y + 1);
					glVertex3f(x + 1,	zn0,	y);	//todo: neighbour;
					
					glVertex3f(x + 1,	zn0,	y);
					glVertex3f(x + 1,	zn3,	y + 1);
					glVertex3f(x + 1,	z[2],	y + 1);
				}
				auto wally = square.walls[1];
				if (wally != Wall::None)
				{
					float zn2 = 0, zn3 = 0;
					auto neighbour = MaybeAt(Position(x, y-1));
					if (neighbour)
					{
						auto neighbourZ = neighbour->Z();
						zn2 = neighbourZ[2];
						zn3 = neighbourZ[3];
					}
					auto wallColor = colorTable[unsigned(wally)];
					glColor4f(wallColor.r, wallColor.g, wallColor.b, wallColor.a);
					glVertex3f(x,		z[0],	y);
					glVertex3f(x + 1,	z[1],	y);
					glVertex3f(x,		zn3,	y);	//todo: neighbour z

					glVertex3f(x,		zn3,	y);
					glVertex3f(x + 1,	zn2,	y);
					glVertex3f(x + 1,	z[1],	y);
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
				glVertex3f(x, z[0], y);
                glVertex3f(x+1, z[1], y);

                glVertex3f(x+1, z[1], y);
                glVertex3f(x+1, z[2], y+1);

                glVertex3f(x+1, z[2], y+1);
                glVertex3f(x, z[3], y+1);

                glVertex3f(x, z[3], y+1);
                glVertex3f(x, z[0], y);
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
        s >> square.height[0] >> square.height[1] >> square.height[2] >> square.height[3];
        return s;
    }


}   // ::Game


