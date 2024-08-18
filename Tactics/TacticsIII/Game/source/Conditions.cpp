#include "Game/Conditions.h"
#include "Utility/String.h"

namespace Game
{

Conditions::Conditions(const json& data)
{
    for(const auto& el: data.items())
    {
        conditions.emplace_back(Condition::Deserialize(el.key(), el.value()));
    }
}


Computation Conditions::Bonus(Stat::Id id) const
{
    Computation result;
    for(const auto& c : conditions)
    {
        result += c->Bonus(id);
    }
    return result;
}

std::string Conditions::Description() const
{
    std::vector<std::string_view> condition_names;
    std::transform(conditions.begin(), conditions.end(), std::back_insert_iterator(condition_names),
    [](const decltype(conditions)::value_type&  c)
    {
        return c->Name();
    });
    return Engine::Join(condition_names, ", ");
}


json Conditions::Serialize() const
{
    auto result = json::object();

    for(const auto& condition: conditions)
    {
        const auto& [key, value] = condition->Serialize();
        result[key] = value;
    }
    return result;
}

}
