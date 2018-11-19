#include "stdafx.h"
#include <numeric>
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

    Score Actor::Strength() const
    {
        return body.Strength();  // todo: equipment boni
    }

    Bonus Actor::StrReqPenalty() const
    {
        int totalRequiredStrength = std::accumulate(worn.begin(), worn.end(), 0, [](int str, const Armor& armor) -> int { return str + armor.Required().strength; });
        totalRequiredStrength += std::accumulate(wielded.begin(), wielded.end(), 0, [](int str, const Weapon& weapon) -> int { return str + weapon.Required().strength; });

        auto strength = Strength();
        int reqPentalty = std::max(0, totalRequiredStrength - int(strength.Value()));
        return Bonus(L"Str(" + strength.Description() + L")-" + std::to_wstring(totalRequiredStrength), -reqPentalty);
    }

    Score Actor::Agility() const
    {
        return body.Agility() + StrReqPenalty();
    }

    Bonus Actor::AgilityMoveBonus() const
    {
        auto agility = Agility();
        if (agility.Value() == 0)
            return Bonus(L"Agi("+agility.Description()+L")", -100);
        else
            return Bonus(L"Agi("+agility.Description()+L")", (int(agility.Value()) - 12) / 2);
    }

    Score Actor::GetMaxMovePoints() const
    {
        // TODO: spell effects
        return Score() + Bonus(10) + AgilityMoveBonus();
    }

    Score Actor::Constitution() const
    {
        return body.Constitution();  // todo: equipment boni
    }

    Score Actor::Intelligence() const
    {
        return body.Intelligence(); // todo: equipment requirements, equipment boni
    }
    Score Actor::Wisdom() const
    {
        return body.Wisdom();  // todo:equipment boni
    }

    Bonus Actor::ConstitutionBonus() const
    {
        auto consitution = Constitution();
        return Bonus(L"Con (" + consitution.Description() + L")", (int(consitution.Value()) - 11) / 3);
    }

    Bonus Actor::StrengthBonus() const
    {
        auto strength = Strength();
        return Bonus(L"Str (" + strength.Description() + L")", (int(strength.Value()) - 11) / 3);
    }

    Bonus Actor::IntelligenceBonus() const
    {
        auto intelligence = Intelligence();
        return Bonus(L"Int (" + intelligence.Description() + L")", (int(intelligence.Value()) - 11) / 3);
    }

    Bonus Actor::WisdomBonus() const
    {
        auto wisdom = Wisdom();
        return Bonus(L"Wis (" + wisdom.Description() + L")", (int(wisdom.Value()) - 11) / 3);
    }

    Bonus Actor::WisReqPenalty() const
    {
        int totalRequiredWisdom = std::accumulate(worn.begin(), worn.end(), 0, [](int str, const Armor& armor) -> int { return str + armor.Required().wisdom; });
        totalRequiredWisdom += std::accumulate(wielded.begin(), wielded.end(), 0, [](int str, const Weapon& weapon) -> int { return str + weapon.Required().wisdom; });

        auto wisdom = Wisdom();
        int reqPentalty = std::max(0, totalRequiredWisdom- int(wisdom.Value()));
        return Bonus(L"Wis("+wisdom.Description() + L")-" + std::to_wstring(totalRequiredWisdom) , -reqPentalty);
    }
    Stats Actor::Statistics() const
    {
        return Stats::Stats(Strength().Value(), Agility().Value(), Constitution().Value(), Intelligence().Value(), Wisdom().Value());
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

    Damage Actor::AttackDamage() const
    {
        if (wielded.empty())
        {
            return Damage(Wound::Type::Blunt, Score(L"Unarmed", 2) + StrengthBonus());
        }
        else
        {
            auto strengthBonus = StrengthBonus();
            auto wisdomBonus = WisdomBonus();
            return wielded.front().Damage() ^ (
                Damage(Wound::Type::Blunt, Score(strengthBonus)) +
                Damage(Wound::Type::Sharp, Score(strengthBonus)) +
                Damage(Wound::Type::Disease, Score(wisdomBonus)) +
                Damage(Wound::Type::Spirit, Score(wisdomBonus)));
        }
    }

    Damage Actor::Mitigation() const
    {
        Score constMitigation(ConstitutionBonus());
        Score wisMitigation(WisdomBonus());
        Damage mitigation= Damage(Wound::Type::Sharp, constMitigation) +
            Damage(Wound::Type::Blunt, constMitigation)+
            Damage(Wound::Type::Burn, constMitigation)+
            Damage(Wound::Type::Disease, constMitigation)+
            Damage(Wound::Type::Spirit, wisMitigation);
        if (!worn.empty())
        {
            // TODO: select right armor for location
            mitigation += worn.front().Mitigation();
        }
        return mitigation;
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