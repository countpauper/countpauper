#pragma once

#include <memory>
#include <istream>
#include <vector>

#include "Direction.h"
#include "Map.h"

namespace Game
{
	class Object;
	struct Position;

    class Game
    {
    public:
        Game();
		virtual ~Game();
        void Render() const;
        void Key(unsigned short code);
		bool CanBe(const Position& position) const;
		bool CanGo(const Position& from, Direction direction) const;
    protected:
        friend std::wistream& operator>>(std::wistream& s, Game& game);
        Map map;
        std::vector<std::unique_ptr<Object>> objects;
        unsigned player;
    };
    std::wistream& operator>>(std::wistream& s, Game& game);

}   // ::Game

