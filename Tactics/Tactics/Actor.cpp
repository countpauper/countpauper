#include "stdafx.h"
#include <numeric>
#include <gl/GL.h>
#include "Actor.h"
#include "Action.h"
#include "Color.h"
#include "Game.h"
#include "Skills.h"
#include "Geometry.h"
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
        const float HALF_PI = M_PI*0.5f;
        glPushMatrix();
        if (Dead())
            glRotatef(90, 1, 0, 0);
        glRotated(Engine::Rad2Deg(direction.Angle()), 0, 1, 0);
        glBegin(GL_TRIANGLES);
        for (unsigned i = 0; i < sides; ++i)
        {
            float a = float(i) / sides * 2.0f * M_PI;
            float b = float(i + 1) / sides * 2.0f * M_PI;
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

    Stats::Score Actor::Strength() const
    {
        return body.Strength();  // todo: health penalty, equipment boni
    }

    Stats::Score Actor::Agility() const
    {
        int totalRequiredStrength = std::accumulate(armors.begin(), armors.end(), 0, [](int str, const Armor& armor) -> int { return str+armor.Required().strength; });
        totalRequiredStrength += std::accumulate(weapons.begin(), weapons.end(), 0, [](int str, const Weapon& weapon) -> int { return str+weapon.Required().strength; });
        int reqPentalty = std::max(0, totalRequiredStrength - int(Strength()));
        return body.Agility()- reqPentalty;// todo: equipment boni
    }

    Stats::Score Actor::Constitution() const
    {
        return body.Constitution();  // todo: equipment boni
    }

    Stats::Score Actor::Intelligence() const
    {
        return body.Intelligence(); // todo: equipment requirements, equipment boni
    }
    Stats::Score Actor::Wisdom() const
    {
        return body.Wisdom();  // todo:equipment boni
    }

    Stats Actor::Statistics() const
    {
        return Stats::Stats(Strength(), Agility(), Constitution(), Intelligence(), Wisdom());
    }
    void Actor::Turn()
    {
        mp = Agility();
    }

    bool Actor::Dead() const
    {
        return body.Dead(Constitution());
    }
    
    bool Actor::CanAct() const
    {
        return !Dead() && mp > 0;
    }

    const Actor::Skills& Actor::GetSkills() const
    {
        return skills;
    }

    unsigned Actor::GetSkillScore(const ::Game::Skill& findSkill) const
    {
        auto it = std::find_if(skills.begin(), skills.end(), [&findSkill](const Actor::Skill& skill) { return skill.skill == &findSkill; });
        if (it == skills.end())
            return 0;
        else
            return it->score;
    }

    Damage Actor::AttackDamage() const
    {
        if (weapons.empty())
            return Damage(0,2,0,0,0);   // unarmed
        else
            return weapons.front().Damage();
    }

    Damage Actor::Mitigation() const
    {
        if (armors.empty())
            return Damage();
        else
            return armors.front().Mitigation();
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
        actor.armors.reserve(armors);
        while (actor.armors.size()<armors)
        {
            std::wstring typeName, materialName, bonusName;
            s >> bonusName >> materialName >> typeName;
            actor.armors.emplace_back(Armor(game, typeName, materialName, bonusName));
        }
        while (actor.weapons.size()<weapons)
        {
            std::wstring typeName, materialName, bonusName;
            s >> bonusName >> materialName >> typeName;
            actor.weapons.emplace_back(Weapon(game, typeName, materialName, bonusName));
        }
        actor.skills.resize(skills);
        for (auto& skill : actor.skills)
        {
            std::wstring skillName;
            s >> skillName >> skill.score;
            skill.skill = game.skills->Find(skillName);
        }

        return s;
    }
} // ::Game