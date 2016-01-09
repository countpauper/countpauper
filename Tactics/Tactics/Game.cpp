#include "stdafx.h"
#include <gl/GL.h>
#include "Game.h"
#include "Map.h"
#include "Actor.h"
#include "Plan.h"

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

            glTranslatef(float(position.x)+0.5f, square.Z(), float(position.y)+0.5f);
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
                plan->Execute(playerActor);
            playerActor.Turn();
            plan.reset();
            return;
        }
        if (Action::keymap.count(code) == 0)
            return;

        if (!plan)
            plan = std::make_unique<Plan>(playerActor);
        std::unique_ptr<Action> action(Action::keymap[code](plan->Final(), *this));
        if (!action)
            return;
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

    Actor* Game::FindTarget(const Position& from) const
    {
        for (auto& object : objects)
        {
            auto actor = dynamic_cast<Actor*>(object.get());
            if (!actor)
                continue;
            if (actor->GetPosition().Distance(from) == 1)    //TODO: more explicitly exclude actor, by team 
                return actor;
        }
        return nullptr;
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
        auto& playerActor = *dynamic_cast<Actor*>(objects.at(player).get());
        if (selection == Selection::Map)
        {
            Position target(value & 0xFFFF, (value >> 16) & 0xFFFF);
            plan.reset(new PathPlan(playerActor, target,*this));
        }
        else if (selection == Selection::Object)
        {
            auto object = objects.at(value).get();
            if (auto target= dynamic_cast<Actor*>(object))
            {
                plan.reset(new AttackPlan(playerActor, *target, *this));
            }
        }
    }
}   // ::Game


