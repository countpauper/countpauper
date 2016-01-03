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
	class Plan;
	class Actor;

    class Game
    {
    public:
		enum class Selection
		{
			None = 0,
			Map,
			Object
		};
        Game();
		virtual ~Game();
        void Render() const;
        void Key(unsigned short code);
		bool CanBe(const Position& position) const;
		bool CanGo(const Position& from, Direction direction) const;
		void Click(Selection selection, uint32_t value);
		Actor* FindTarget(const Position& from) const;
    protected:
		Plan* PathPlan(const Actor& actor, const Position& target) const;
	protected:
		friend std::wistream& operator>>(std::wistream& s, Game& game);
        Map map;
        std::vector<std::unique_ptr<Object>> objects;
        unsigned player;
		std::unique_ptr<Plan> plan;
    };
    std::wistream& operator>>(std::wistream& s, Game& game);

}   // ::Game

