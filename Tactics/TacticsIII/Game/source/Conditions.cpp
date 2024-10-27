#include "Game/Conditions.h"
#include "Utility/String.h"

namespace Game
{

Conditions::Conditions(const json& data)
{
    for(const auto& el: data.items())
    {
        conditions.emplace(std::make_pair(Condition::Find(el.key()), el.value()));
    }
}


Computation Conditions::Bonus(Stat::Id id) const
{
    Computation result;
    for(const auto& c : conditions)
    {
        // NB: Powers is not factored in here. Some conditions (spell effects)
        // will downgrade when their power is diminished (by time/healing)
        result += c.first->Bonus(id);
    }
    return result;
}

std::string Conditions::Description() const
{
    std::vector<std::string_view> condition_names;
    std::transform(conditions.begin(), conditions.end(), std::back_insert_iterator(condition_names),
    [](const decltype(conditions)::value_type&  c)
    {
        return c.first->Name();
    });
    return Engine::Join(condition_names, ", ");
}


json Conditions::Serialize() const
{
    auto result = json::object();

    for(const auto& condition: conditions)
    {
        result[condition.first->Name()] = condition.second;
    }
    return result;
}

}
