#include "stdafx.h"
#include "Skills.h"
#include "Attack.h"

namespace Game
{
    Skill::Skill() :
        cost(0),
        range(0)
    {
    }
    Action* Skill::Action(Actor& target) const
    {
        return new Attack(target,*this);
    }
    std::wistream& operator>>(std::wistream& s, Skill& skill)
    {
        std::wstring type;
        s >> type >> skill.name >> skill.vector;
        s >> skill.cost >> skill.range;
        return s;
    }

    const Skill* Skills::Find(const std::wstring& name) const
    {
        for (auto& skill : *this)
            if (skill.name == name)
                return &skill;
        return nullptr;
    }
    std::wistream& operator>>(std::wistream& s, Skills& skills)
    {
        while (!s.eof())
        {
            if (s.bad())
                break;
            Skill skill;
            s >> skill;
            skills.emplace_back(skill);
        }
        return s;
    }

}