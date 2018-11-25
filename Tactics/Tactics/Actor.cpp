#include "stdafx.h"
#include <gl/GL.h>
#include "Engine/Color.h"
#include "Engine/Geometry.h"
#include "Actor.h"
#include "Action.h"
#include "Game.h"
#include "Skills.h"

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

    Position Object::GetPosition() const
    {
        return position;
    }

    Actor::Actor() :
        mp(0),
        team(0)
    {
    }

    void Actor::Render() const
    {
        unsigned sides = 16;
        float r = 0.25f;

        Engine::RGBA teamColor[] = {
            { 255, 0, 0, 255 },
            { 0, 255, 0, 255 },
            { 0, 0, 255, 255 }
        };
        const float HALF_PI = float(Engine::PI)*0.5f;
        glPushMatrix();
        if (Dead())
            glRotatef(90, 1, 0, 0);
        glRotated(Engine::Rad2Deg(direction.Angle()), 0, 1, 0);
        glBegin(GL_TRIANGLES);
        for (unsigned i = 0; i < sides; ++i)
        {
            float a = float(i) / sides * 2.0f * float(Engine::PI);
            float b = float(i + 1) / sides * 2.0f * float(Engine::PI);
            teamColor[team].Render();

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

    void Actor::Turn()
    {
        auto mps = GetMaxMovePoints();
        mp = mps.Value();
        OutputDebugStringW((L"Turn " + name + L" MP=" + mps.Description() + L"\r\n").c_str());
    }

    bool Actor::Dead() const
    {
        return body.Dead();
    }
    
    bool Actor::CanAct() const
    {
        return !Dead() && mp > 0;
    }

    const Actor::Skills& Actor::GetSkills() const
    {
        return skills;
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

    std::vector<const Weapon*> Actor::Wielded() const
    {
        std::vector<const Weapon*> result(wielded.size());
        std::transform(wielded.begin(), wielded.end(), result.begin(), [](const Weapon& item)
        {
            return &item;
        });
        return result;
    }

    const ::Game::Skill* Actor::DefaultAttack() const
    {
        for (auto& skill : skills)
        {
            if ((skill.skill->IsActive()) && 
                (IsPossible(*skill.skill)))
                return skill.skill;
        }
        return nullptr;
    }

    bool Actor::IsPossible(const ::Game::Skill& skill) const
    {
        if ((skill.weapon != Type::Weapon::Style::None) &&
            (std::none_of(wielded.begin(), wielded.end(), [&skill](const Weapon& weapon)
        {
            return weapon.Match(skill.weapon);
        })))
        {
            return false;
        }
        return true;
    }

    unsigned Actor::GetSkillScore(const ::Game::Skill& findSkill) const
    {
        auto it = std::find_if(skills.begin(), skills.end(), [&findSkill](const Actor::Skill& skill) { return skill.skill == &findSkill; });
        if (it == skills.end())
            return 0;
        else
            return it->score;
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
            std::wstring typeName, materialName, bonusName;
            s >> bonusName >> materialName >> typeName;
            actor.wielded.emplace_back(Weapon(game, typeName, materialName, bonusName));
        }
        actor.skills.resize(skills);
        for (auto& skill : actor.skills)
        {
            std::wstring skillName;
            s >> skillName >> skill.score;
            skill.skill = game.skills.Find(skillName);
        }

        return s;
    }
} // ::Game