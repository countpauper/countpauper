#include "stdafx.h"
#include <gl/GL.h>
#include "Engine/Text.h"
#include "Engine/Geometry.h"
#include "Actor.h"
#include "Action.h"
#include "Plan.h"
#include "Game.h"
#include "Skills.h"
#include "Engine/Coordinate.h"

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

    void Actor::Activate(const Game& game)
    {
        assert(!active);
        auto mps = GetMaxMovePoints();
        mp = mps.Value();
        if (CanAct())
        {
            active = true;
            randomState = Engine::Random().Store();
            OutputDebugStringW((L"Turn " + name + L" MP=" + mps.Description() + L"\r\n").c_str());
        }
        else
        {
            active = false;
            OutputDebugStringW((L"Skip " + name + L" MP=" + mps.Description() + L"\r\n").c_str());
        }
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

    bool Actor::IsEngaged() const
    {
        return ((plan) && (plan->Engaging()));
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
                (IsPossible(*skill)))
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

    void Actor::RestoreRandom()
    {
        Engine::Random().Restore(randomState);
    }

    void Actor::Execute(Game& game)
    {
        if (plan)
        {
            if (plan->Anticipating())
                active = false;
            else
            {
                RestoreRandom();
                plan->Compute(game);
                if (plan->Execute(game))
                {
                    if (plan->Engaging())
                        active = false;
                    plan.reset();
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
        return !Dead() && mp > 0;
    }

    const Actor::Knowledge& Actor::GetSkills() const
    {
        return knowledge;
    }

    std::vector<const Skill*> Actor::FollowSkill(const Skill& previous, Skill::Trigger trigger) const
    {
        std::vector<const Skill*> possible;
        for (auto skill : GetSkills())
        {
            if ((skill->trigger == trigger) && (skill->Follows(previous)))
            {
                possible.push_back(skill);
            }
        }
        return possible;
    }


    std::vector<const Armor*> Actor::Worn() const
    {
        std::vector<const Armor*> result(worn.size());
        std::transform(worn.begin(), worn.end(), result.begin(), [](const Armor& item)
        {
            return &item;
        });
        return result;
    }

    std::map<const Body::Part*, const Weapon*> Actor::Wielded() const
    {
        std::map<const Body::Part*, const Weapon*> result;

        std::transform(wielded.begin(), wielded.end(), std::inserter(result, result.end()), [](const decltype(wielded)::value_type& wield)
        {
            return std::make_pair(wield.first, &wield.second);
        });
        auto hands = body.Grip();
        for (auto hand : hands)
        {
            result.insert(std::pair<const Body::Part*, const Weapon*>(hand, nullptr));
        }
        return result;
    }

    const Skill* Actor::DefaultAttack() const
    {
        for (auto skill : knowledge)
        {
            if ((skill->IsAttack()) && 
                (IsPossible(*skill)))
                return skill;
        }
        return nullptr;
    }

    const Skill* Actor::DefaultMove() const
    {
        for (auto skill : knowledge)
        {
            if ((skill->IsMove()) &&
                (IsPossible(*skill)))
                return skill;
        }
        return nullptr;
    }

    const Skill* Actor::WaitSkill() const
    {
        for (auto skill: knowledge)
        {
            if ((skill->IsWait()) &&
                (IsPossible(*skill)))
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
                (IsPossible(*skill)))
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

    bool Actor::IsPossible(const Skill& skill) const
    {
        if (skill.weapon == Type::Weapon::Style::All)
            return true;
        if (skill.weapon == Type::Weapon::Style::None)
            return wielded.empty();
        return (std::any_of(wielded.begin(), wielded.end(), [&skill](const decltype(wielded)::value_type& pair)
        {
            return pair.second.Match(skill.weapon);
        }));
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
            actor.worn.emplace_back(Armor(game, typeName, materialName, bonusName));
        }
        while (actor.wielded.size()<weapons)
        {
            std::wstring limbName, typeName, materialName, bonusName;
            s >> limbName >> bonusName >> materialName >> typeName;
            auto limb = actor.body.Get(limbName);
            if (!limb)
                throw std::runtime_error("Wielded limb not found");
            actor.wielded.emplace(std::make_pair(limb,Weapon(game, typeName, materialName, bonusName)));
        }
        actor.knowledge.resize(skills);
        for (auto& skill : actor.knowledge)
        {
            std::wstring skillName;
            s >> skillName;
            skill = game.skills.Find(skillName);
        }

        return s;
    }
} // ::Game