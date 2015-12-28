#pragma once

#include <memory>
#include <istream>
#include <vector>

namespace Game
{
	class Map;
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
    protected:
        friend std::wistream& operator>>(std::wistream& s, Game& game);
        std::unique_ptr<Map> map;
        std::vector<std::unique_ptr<Object>> objects;
        unsigned player;
    };
    std::wistream& operator>>(std::wistream& s, Game& game);

}   // ::Game

