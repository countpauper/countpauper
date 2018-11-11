#include "stdafx.h"
#include "CSV.h"
#include "Skills.h"
#include "Attack.h"

namespace Game
{

std::map<std::wstring, Skill::Trigger> triggerMap(
{
    { L"", Skill::Trigger::None },
    { L"Act", Skill::Trigger::Act },
    { L"React", Skill::Trigger::React },
    { L"Defend", Skill::Trigger::Defend },
});

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
    Engine::Adapter::Enumeration<Skill, Skill::Trigger> trigger(&Skill::trigger, triggerMap);
    Engine::Adapter::Set<Skill, std::string> label(&Skill::label);
    Engine::Adapter::Set<Skill, std::string> follows(&Skill::follows);

    Engine::CSV<Skill> csv(file, { &name, &mp, &trigger, &label, &follows });
    return csv.Read();
}

Skills::Skills(const std::vector<Skill>& skills):
    vector<Skill>(skills)
{
}

}