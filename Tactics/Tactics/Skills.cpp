#include "stdafx.h"
#include "CSV.h"
#include "Skills.h"
#include "Attack.h"

namespace Game
{
Skill::Skill() :
    mp(0),
    range(1.0)
{
}

Action* Skill::Action(Actor& target) const
{
    return new Attack(target, *this);
}

const Skill* Skills::Find(const std::wstring& name) const
{
    for (auto& skill : *this)
        if (skill.name == name)
            return &skill;
    return nullptr;
}

Skills Skills::Load(std::wistream& file)
{
    Engine::Adapter::String<Skill> name(&Skill::name);
    Engine::Adapter::Unsigned<Skill> mp(&Skill::mp);

    Engine::CSV<Skill> csv(file, { &name, &mp });
    return csv.Read();
}

Skills::Skills(const std::vector<Skill>& skills):
    vector<Skill>(skills)
{
}

}