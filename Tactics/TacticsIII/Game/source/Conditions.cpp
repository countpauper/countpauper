#include "Game/Conditions.h"
#include "Utility/String.h"

namespace Game
{

StatDescriptor& Conditions::Contribute(Stat::Id id, StatDescriptor& result) const
{
    for(const auto& c : conditions)
    {
        result = result.Contribute(c->Name(), c->Contribution(id));
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

}
