#include "stdafx.h"
#include <gl/GL.h>
#include "Game.h"
#include "Map.h"
#include "Actor.h"
#include "Action.h"

namespace Game
{

    Game::Game() :
        player(0)
    {
    }
	
	Game::~Game() = default;


    void Game::Render() const
    {
        map.Render();
		for (const auto& object : objects)
		{
			glPushMatrix();
			auto position = object->GetPosition();
			auto square = map.At(position);

			glTranslated(static_cast<float>(position.x)+0.5, square.Z(), static_cast<float>(position.y)+0.5);
			object->Render();
			glPopMatrix();
		}
    }

    void Game::Key(unsigned short code)
    {
		if (Action::keymap.count(code) == 0)
			return;
		std::unique_ptr<Action> action(Action::keymap[code]());
        auto& playerActor = *dynamic_cast<Actor*>(objects.at(player).get());
		auto result = action->Act(playerActor, *this);
		if (!result.possible)
			return;
		playerActor.Apply(result);
    }

	bool Game::CanBe(const Position& position) const
	{
		if (!map.CanBe(position))
			return false;
		for (const auto& object : objects)
			if (object->GetPosition() == position)
				return false;
		return true;
	}

	bool Game::CanGo(const Position& from, Direction direction) const
	{
		return map.CanGo(from, direction);
	}

	std::wistream& operator>>(std::wistream& s, Game& game)
    {
        unsigned objects;
        s >> objects;
        for (unsigned oi = 0; oi < objects; ++oi)
        {
            std::wstring type;
            s >> type;
            if (type == L"Actor")
            {
                auto actor = std::make_unique < Actor>();
                s >> *actor;
                game.objects.emplace_back(std::move(actor));

            }
        }
        s >> game.map;
        return s;
    }

}   // ::Game


