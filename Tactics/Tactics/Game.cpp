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
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glPushName(GLuint(Selection::Map));
        map.Render();
		glPopName();
		glPushName(GLuint(Selection::Object));
		unsigned index = 0;
		for (const auto& object : objects)
		{
			glPushMatrix();
			auto position = object->GetPosition();
			auto square = map.At(position);

			glTranslated(static_cast<float>(position.x)+0.5, square.Z(), static_cast<float>(position.y)+0.5);
			glPushName(index++);
			object->Render();
			glPopName();
			glPopMatrix();
		}
		glPopName();
		if (plan)
			plan->Render();
    }

    void Game::Key(unsigned short code)
    {
		if (code == VK_ESCAPE)
		{
			plan.reset();
			return;
		}
		auto& playerActor = *dynamic_cast<Actor*>(objects.at(player).get());
		if (code == VK_RETURN)
		{
			if (plan)
				playerActor.Apply(plan->Final());
			playerActor.Turn();
			plan.reset();
			return;
		}
		if (Action::keymap.count(code) == 0)
			return;

		std::unique_ptr<Action> action(Action::keymap[code]());
		if (!plan)
			plan = std::make_unique<Plan>(playerActor);
		State state(plan->Final());
		auto result = action->Act(state, *this);
		if (!result.possible)
			return;
		plan->Add(std::move(action), result);
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

	void Game::Click(Selection selection, uint32_t value)
	{
		if (selection == Selection::Map)
		{
			Position target(value & 0xFFFF,(value >> 15) & 0xFFFF);
			plan.reset(PathPlan(target));
		}
	}

	Plan* Game::PathPlan(const Position&target) const
	{
		return nullptr;
	}
}   // ::Game


