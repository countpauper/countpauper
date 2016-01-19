#include "stdafx.h"
#include "Skills.h"
#include "Action.h"

namespace Game
{
    Skill::Skill() :
        cost(0),
        range(0)
    {
    }
    std::unique_ptr<Action> Skill::Action() const
    {
        return nullptr;
    }
    std::wistream& operator>>(std::wistream& s, Skill& skill)
    {
        std::wstring type;
        s >> type >> skill.name;
        s >> skill.cost >> skill.range;
        return s;
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