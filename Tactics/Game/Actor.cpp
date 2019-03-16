#include "stdafx.h"
#include <gl/GL.h>
#include "Engine/Text.h"
#include "Engine/Geometry.h"
#include "Engine/Coordinate.h"
#include "Game/Skills.h"
#include "Actor.h"
#include "Action.h"
#include "Plan.h"
#include "Game.h"

namespace Game
{
    Object::Object()
    {

    }
    void Object::Move(int dx, int dy)
    {
        position.x = std::min(std::max(0, static_cast<int>(position.x) + dx), 5);
        position.y = std::min(std::max(0, static_cast<int>(position.y) + dy), 5);
    }

    unsigned Object::Id() const
    {
        static_assert(sizeof(unsigned) == sizeof(this), "Failed to use object pointer as id");
        return unsigned(this);
    }


    bool Object::Prone() const
    {
        return true;
    }


    Position Object::GetPosition() const
    {
        return position;
    }
    std::wstring Object::Description() const
    {
        return name;
    }


    Actor::Actor() :
        active(false),
        mp(0),
        team(0)
    {
    }

    void Actor::Render() const
    {
        unsigned sides = 16;
        float r = 0.25f;
        glPushMatrix();
        glTranslatef(float(position.x) + 0.5f, 0.0, float(position.y) + 0.5f);

        Game::teamColor[team].Render();
        if (Prone())
        {
            glRotatef(90, 1, 0, 0);
        }
        glPushMatrix();
            glTranslatef(-0.5, 0, -1.5);

            Engine::Font::default.Select();
            Engine::glText(name);
        glPopMatrix();

        glRotated(Engine::Rad2Deg(direction.Angle()), 0, 1, 0);
        glBegin(GL_TRIANGLES);
        for (unsigned i = 0; i < sides; ++i)
        {
            float a = float(i) / sides * 2.0f * float(Engine::PI);
            float b = float(i + 1) / sides * 2.0f * float(Engine::PI);

            float xa = cos(a) * r;
            float ya = sin(a) * sin(a*0.5f) * r;
            float xb = cos(b) * r;
            float yb = sin(b) * sin(b*0.5f) * r;

            glNormal3f(0.0f, -1.0f, 0.0f);
            glVertex3f(0, 0, 0);
            glVertex3f(xb, 0, yb);
            glVertex3f(xa, 0, ya);

            glNormal3f(cos(a), 0.0f, sin(a));
            glVertex3f(xa, 1, ya);
            glVertex3f(xa, 0, ya);
            glNormal3f(cos(b), 0.0f, sin(b));
            glVertex3f(xb, 0, yb);

            glNormal3f(cos(a), 0.0f, sin(a));
            glVertex3f(xa, 1, ya);
            glNormal3f(cos(b), 0.0f, sin(b));
            glVertex3f(xb, 0, yb);
            glVertex3f(xb, 1, yb);

            glNormal3f(0.0f, 1.0f, 0.0f);
            glVertex3f(0, 1, 0);
            glVertex3f(xa, 1, ya);
            glVertex3f(xb, 1, yb);
        }
        glEnd();
        glPopMatrix();
        if (plan)
            plan->Render();
    }

    unsigned Actor::GetMovePoints() const
    {
        return mp;
    }

    unsigned Actor::GetTeam() const
    {
        return team;
    }

    Direction Actor::GetDirection() const
    {
        return direction;
    }
    void Actor::Apply(const State& result)
    {
        position = result.position;
        direction = result.direction;
        mp = result.mp;
        body = result.body;
    }


    Bonus Actor::AgilityMoveBonus() const
    {
        State state(*this);
        auto agility = state.Agility();
        if (agility.Value() == 0)
            return Bonus(L"Agi(" + agility.Description() + L")", -100);
        else
            return Bonus(L"Agi(" + agility.Description() + L")", (int(agility.Value()) - 12) / 2);
    }

    Score Actor::GetMaxMovePoints() const
    {
        // TODO: spell effects
        return Score() + Bonus(10) + AgilityMoveBonus();
    }

    bool Actor::IsAlly(const Actor& other) const
    {
        return GetTeam() == other.GetTeam();
    }

    void Actor::Turn()
    {
        body.Disengage();
    }

    void Actor::Activate(const Game& game)
    {
        assert(!active);
        if (Dead())
        {
            mp = 0;
            OutputDebugStringW((L"Dead " + name + L"\r\n").c_str());
        }
        else
        {
            auto mps = GetMaxMovePoints();
            // carry over at most 1 point from the previous turn, so very slow characters can act every other turn at least
            mps+=Bonus(L"Carry", std::min(static_cast<int>(mp), 1));
            mp = mps.Value();
            if (CanAct())
            {
                active = true;
                OutputDebugStringW((L"Turn " + name + L" MP=" + mps.Description() + L"\r\n").c_str());
            }
            else
            {
                active = false;
                OutputDebugStringW((L"Skip " + name + L" MP=" + mps.Description() + L"\r\n").c_str());
            }
        }
    }


    void Actor::Deactivate()
    {
        active = false;
    }

    bool Actor::Trigger(const Actor& actor, Game& game)
    {
        if (!plan)
            return false;
        if (plan->Trigger(actor))
        {
            active = true;
            Execute(game);
            return true;
        }
        return false;
    }

    bool Actor::IsActive() const
    {
        return active;
    }

    bool Actor::IsIdle() const
    {
        return active && !plan;
    }

    bool Actor::IsAnticipating() const
    {
        return ((plan) && (plan->Anticipating()));
    }

    void Actor::AI(Game& game)
    {
        std::vector<std::unique_ptr<Plan>> plans;
        for (auto skill : GetSkills())
        {
            if ((skill->IsAttack()) &&
                (IsSelectable(*skill)))
            {
                auto targets = game.FindTargets(*this, *skill);
                for (auto target : targets)
                {
                    auto option = std::make_unique<AttackPlan>(*this, *target, game, *skill);
                    if (option->Valid())
                        plans.emplace_back(std::move(option));
                }
            }
        }
        if (!plans.empty())
            plan = std::move(plans.front());
        else
            plan = std::make_unique<SkipPlan>(*this, game);
    }

    void Actor::Execute(Game& game)
    {
        if (plan)
        {
            if (plan->Anticipating())
                Deactivate();
            else
            {
                plan->Compute(game);
                if (plan->Execute(game))
                {
                    plan.reset();
                    if (!CanAct())
                        Deactivate();
                }
            }
        }
    }


    bool Actor::Dead() const
    {
        return body.Dead();
    }

    bool Actor::Prone() const
    {
        return direction.Prone();
    }
    
    bool Actor::CanAct() const
    {
        for (auto skill : knowledge)
        {
            if (skill->IsWait())
                continue;
            if (IsSelectable(*skill))
                return true;
        }
        return false;
    }

    const Actor::Knowledge& Actor::GetSkills() const
    {
        return knowledge;
    }

    std::set<const Skill*> Actor::Counters(const Skill& previous) const
    {
        std::set<const Skill*> possible;
        std::copy_if(knowledge.begin(), knowledge.end(), std::inserter(possible, possible.end()), [&previous](const decltype(knowledge)::value_type& skill)
        {
            return skill->Counter(previous);
        });
        return possible;
    }

    std::set<const Skill*> Actor::Combos(const Skill& previous) const
    {
        std::set<const Skill*> possible;
        std::copy_if(knowledge.begin(), knowledge.end(), std::inserter(possible, possible.end()), [&previous](const decltype(knowledge)::value_type& skill)
        {
            return skill->Combo(previous);
        });
        return possible;
    }

    std::vector<const Armor*> Actor::Worn() const
    {
        std::vector<const Armor*> result(worn.size());
        std::transform(worn.begin(), worn.end(), result.begin(), [](const decltype(worn)::value_type &item)
        {
            return item.get();
        });
        return result;
    }

    std::vector<const Weapon*> Actor::Carried() const
    {
        std::vector<const Weapon*> result(carried.size());
        std::transform(carried.begin(), carried.end(), result.begin(), [](const decltype(carried)::value_type& item)
        {
            return item.get();
        });
        return result;
    }

    std::map<const Body::Part*, const Weapon*> Actor::Wielded() const
    {
        return body.Wielded();
    }

    const Skill* Actor::DefaultAttack() const
    {
        for (auto skill : knowledge)
        {
            if ((skill->IsAttack()) && 
                (IsSelectable(*skill)))
                return skill;
        }
        return nullptr;
    }

    const Skill* Actor::DefaultMove() const
    {
        for (auto skill : knowledge)
        {
            if ((skill->IsMove()) &&
                (IsSelectable(*skill)))
                return skill;
        }
        return nullptr;
    }

    const Skill* Actor::WaitSkill() const
    {
        for (auto skill: knowledge)
        {
            if ((skill->IsWait()) &&
                (IsSelectable(*skill)))
                return skill;
        }
        return nullptr;
    }

    std::vector<std::unique_ptr<Action>> Actor::AllMoves(const Position& from) const
    { 
        std::vector<std::unique_ptr<Action>> result;
        for (auto skill : knowledge)
        {
            if ((skill->IsMove()) &&
                (IsSelectable(*skill)))
            {
                int range = skill->range;
                for (int y = -range; y <= range; ++y)
                {
                    for (int x = -range; x <= range; ++x)
                    { 
                        Position vector(x, y);
                        if (vector.SizeEl() > skill->range)
                            continue;
                        if (!vector)
                            continue;
                        for (auto trajectory : skill->trajectory)
                        {
                            result.emplace_back(skill->CreateAction(*this, Destination(from + vector), trajectory));
                        }
                    }
                }
            }
        }
        return result;
    }

    bool Actor::IsSelectable(const Skill& skill) const
    {
        if (skill.IsWait())
            return true;
		if (!skill.IsActive())
			return false;
        if (!body.Ready(skill))
            return false;   // TODO: should otherwise check Kinetic chain
        return true;
    }

    std::wistream& operator>>(std::wistream& s, Actor& actor)
    {
        Game& game= *static_cast<Game*>(s.pword(1));
        s >> actor.name;
        s >> actor.team;
        s >> actor.position.x >> actor.position.y;
        s >> actor.mp;
        s >> actor.body;
        unsigned armors, weapons, skills;
        s >> armors >> weapons >> skills;
        actor.worn.reserve(armors);
        while (actor.worn.size()<armors)
        {
            std::wstring typeName, materialName, bonusName;
            s >> bonusName >> materialName >> typeName;
            actor.worn.emplace_back(std::make_unique<Armor>(game, typeName, materialName, bonusName));
        }
        while (actor.carried.size()<weapons)
        {
            std::wstring limbName, typeName, materialName, bonusName;
            s >> limbName >> bonusName >> materialName >> typeName;
            actor.carried.emplace_back(std::make_unique<Weapon>(game, typeName, materialName, bonusName));
            if (limbName != L"None")
            {
                auto limb = actor.body.Get(limbName);
                if (!limb)
                    throw std::runtime_error("Wielded limb not found");
                actor.body[*limb].Hold(*actor.carried.back());
            }
        }
        for (size_t n = 0; n < skills; ++n)
        {
            std::wstring skillName;
            s >> skillName;
            auto skill = game.skills.Find(skillName);
            if (!skill)
                throw std::runtime_error("Unknown actor skill");
            actor.knowledge.insert(skill);
        }

        return s;
    }
} // ::Game