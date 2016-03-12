#include "stdafx.h"
#include <gl/GL.h>
#include "Game.h"
#include "Map.h"
#include "Actor.h"
#include "Plan.h"
#include "Skills.h"

namespace Game
{

    Game::Game(std::unique_ptr<Skills> skills) :
        skills(std::move(skills))
    {
    }
    
    Game::~Game() = default;

    State Game::Get(const Actor& actor) const
    {
        return State(actor);
    }
    void Game::Adjust(Actor& actor, const State& state)
    {
        actor.Apply(state);
    }
    void Game::Apply()
    {

    }
    void Game::Tick()
    {
        Actor* actor;
        while ((actor=ActiveActor()) &&
            (!actor->CanAct()))
            Next();
        if (actor->GetTeam() > 0)
            AI(actor);
    }

    void Game::AI(Actor* actor)
    {
        std::vector<std::unique_ptr<Plan>> plans;
        for (auto skill : actor->GetSkills())
        {
            auto targets = FindTargets(*actor, *skill.skill);
            for (auto target : targets)
            {
                plans.emplace_back(std::make_unique<AttackPlan>(*actor, *target, *this, *skill.skill));
            }
        }
        if (plans.size())
            plans.front()->Execute(*this);
        Next();
    }
    void Game::Next()
    {
        if (turn == objects.end())
            return;
        (*turn)->Turn();
        ++turn;
        if (turn == objects.end())
            turn = objects.begin();
    }

    Actor* Game::ActiveActor()
    {
        if (turn == objects.end())
            return nullptr;
        return dynamic_cast<Actor*>(turn->get());
    }

    std::vector<Actor*> Game::FindTargets(const State& from, const Skill& skill) const
    {
        std::vector<Actor*> result;
        for (auto& object : objects)
        {
            auto actor = dynamic_cast<Actor*>(object.get());
            if (!actor)
                continue;
            if (actor->GetTeam() == from.loyalty)
                continue;
            if (actor->Dead())
                continue;
            result.push_back(actor);
        }
        return result;
    }

    std::vector<Actor*> Game::FindTargetsInRange(const State& from, const Skill& skill) const
    {
        auto targets = FindTargets(from, skill);
        auto it = std::remove_if(targets.begin(), targets.end(), [from, skill](const Actor* actor)
        {
            return actor->GetPosition().Distance(from.position) > skill.range;
        });
        targets.erase(it, targets.end());
        return targets;
    }

    void Game::MakePlan(Actor& actor, const Skill& skill)
    {
        auto targets = FindTargets(actor, skill);
        if (targets.size()) 
            plan.reset(new AttackPlan(actor, *targets.front(), *this, skill));
        // TODO: make plan for each target and select the best (shortest?)
        // TODO: append to current plan 
    }

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
            static_assert(sizeof(GLuint) == sizeof(Object*), "Failed to push pointer as name");
            glPushName(GLuint(object.get()));
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
        auto& playerActor = *ActiveActor();
        if (code == VK_RETURN)
        {
            if (plan)
                plan->Execute(*this);
            playerActor.Turn();
            Next();
            plan.reset();
            return;
        }
        if (Action::keymap.count(code) == 0)
            return;

        if (!plan)
            plan = std::make_unique<ManualPlan>(playerActor);
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

    bool Game::Cover(const Position& from, const Position& to) const
    {
        Position delta = to - from;
        if (std::abs(delta.x) < std::abs(delta.y))
        {
            float x = float(from.x);
            float dx = float(delta.x) / float(delta.y);
            if (to.y > from.y)
            {
                for (int y = from.y; y < to.y; ++y)
                {
                    if (!CanGo(Position(std::lroundf(x), y), Direction(Direction::Value::South)))
                        return true;
                    x += dx;
                }
            }
            else
            {
                for (int y = from.y; y < to.y; --y)
                {
                    if (!CanGo(Position(std::lroundf(x), y), Direction(Direction::Value::North)))
                        return true;
                    x += dx;
                }
            }
        }
        else
        {
            float y = float(from.y);
            float dy = float(delta.y) / float(delta.x);
            if (to.x > from.x)
            {
                for (int x = from.x; x < to.x; ++x)
                {
                    if (!CanGo(Position(x, std::lroundf(y)), Direction(Direction::Value::East)))
                        return true;
                    y += dy;
                }
            }
            else
            {
                for (int x = from.x; x < to.x; --x)
                {
                    if (!CanGo(Position(x, std::lroundf(y)), Direction(Direction::Value::West)))
                        return true;
                    y += dy;
                }
            }
        }
        return false;
    }

    std::wistream& operator>>(std::wistream& s, Game& game)
    {
        const int index = 1;
        s.pword(1) = &game;

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
        game.turn = game.objects.begin();
        s >> game.map;
        return s;
    }

    void Game::Click(Selection selection, GLuint value)
    {
        auto& playerActor = *ActiveActor();
        if (selection == Selection::Map)
        {
            Position target(value & 0xFFFF, (value >> 16) & 0xFFFF);
            plan.reset(new PathPlan(playerActor, target,*this));
        }
        else if (selection == Selection::Object)
        {
            auto currentPlan = dynamic_cast<AttackPlan*>(plan.get());
            const Skill* skill = nullptr;
            if (currentPlan)
                skill = &currentPlan->skill;
            else
                skill = playerActor.GetSkills().front().skill;
            if (skill)
            {
                auto object = (Object*)value;
                if (auto target = dynamic_cast<Actor*>(object))
                {
                    plan.reset(new AttackPlan(playerActor, *target, *this, *skill));
                }
            }
        }
    }
}   // ::Game


