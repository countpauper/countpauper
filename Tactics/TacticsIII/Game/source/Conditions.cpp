#include "Game/Conditions.h"
#include "Utility/String.h"

namespace Game
{



unsigned Conditions::GetCondition(const Condition& condition) const
{
    return GetCondition([&condition](const Condition& c) { return &c == &condition; });
}

bool Conditions::Has(std::function<bool(const Condition& condition)> pred) const
{
    return GetCondition(pred) > 0;
}

Computation Conditions::Bonus(Stat::Id id) const
{
    return ConditionalBonus(id);
}

ConditionLevels::ConditionLevels(const json& data)
{
    for(const auto& el: data.items())
    {
        conditions.emplace(std::make_pair(Condition::Find(el.key()), el.value()));
    }
}

ConditionLevels::ConditionLevels(const ConditionLevels& other) :
    conditions(other.conditions)
{
}

ConditionLevels::ConditionLevels(ConditionLevels&& other) :
    conditions(std::move(other.conditions))
{
}

Computation ConditionLevels::ConditionalBonus(Stat::Id id) const
{
    Computation result;
    for(const auto& c : conditions)
    {
        if (!c.second)
            continue;
        // NB: Powers is not factored in here. Some conditions (spell effects)
        // will downgrade when their power is diminished (by time/healing)
        result += c.first->Bonus(id);
    }
    return result;
}


std::pair<const Condition*, unsigned> ConditionLevels::FindCondition(const std::function<bool(const Condition& condition)>& pred) const
{
    auto it = std::find_if(conditions.begin(), conditions.end(), [&pred](const decltype(conditions)::value_type& c) {
        return pred(*c.first);
    });
    if (it!=conditions.end())
    {
        return *it;
    }
    return {nullptr, 0};
}

unsigned ConditionLevels::GetCondition(std::function<bool(const Condition& condition)> pred) const
{
    return FindCondition(pred).second;
}

void ConditionLevels::RemoveCondition(const Condition& condition)
{
    conditions.erase(&condition);
}

void ConditionLevels::InsertCondition(const Condition& condition, unsigned level)
{
    auto [it, inserted] = conditions.insert(std::make_pair(&condition,level));
    if (!inserted)
    {
        it->second = level;
    }
}

void ConditionLevels::SetCondition(const Condition& condition, unsigned level)
{
    if (level==0)
    {
        RemoveCondition(condition);
    }
    else 
    {
        InsertCondition(condition, level); 
    }

}


Conditions::Range ConditionLevels::GetConditions() const
{
    return conditions;
}

void Conditions::ApplyConditions(Conditions::Range conditions)
{
    for(const auto& condition: conditions)
    {
        SetCondition(*condition.first, condition.second);
    }
}

std::string ConditionLevels::Description() const
{
    std::vector<std::string_view> condition_names;
    std::transform(conditions.begin(), conditions.end(), std::back_insert_iterator(condition_names),
    [](const decltype(conditions)::value_type&  c)
    {
        return c.first->Name();
    });
    return Engine::Join(condition_names, ", ");
}


json ConditionLevels::Serialize() const
{
    auto result = json::object();

    for(const auto& condition: conditions)
    {
        result[condition.first->Name()] = condition.second;
    }
    return result;
}

}
