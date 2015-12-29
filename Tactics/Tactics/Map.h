#pragma once
#include <vector>
#include <array>
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
        unsigned short height[4];
        unsigned short reserved;
		std::array<float,4> Z() const;
		float MiddleZ() const;
    };


    class Map
    {
    public:
        Map();
		Square At(const Position& p) const;
		const Square* MaybeAt(const Position& p) const;
		void Render() const;
		bool CanBe(const Position& position) const;
		bool CanGo(const Position& from, Direction direction) const;
	private:
        friend std::wistream& operator>>(std::wistream& s, Map& map);
        std::wstring name;
        unsigned width;
        unsigned height;
        std::vector<Square> squares;
    };


    std::wistream& operator>>(std::wistream& s, Map& map);
    std::wistream& operator>>(std::wistream& s, Square& square);

}   // ::Game



