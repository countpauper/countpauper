#include "Game/Item.h"
#include "Utility/from_string.h"

namespace Game
{

using json = nlohmann::json;
Item::Item() = default;

static std::map<std::string_view, Restriction> restrictionMapping {
    {"", Restriction::none},
    {"cloth", Restriction::cloth},
    {"leather", Restriction::leather},
    {"metal", Restriction::metal},
    {"wood", Restriction::wood},
    {"air", Restriction::air},
    {"stone", Restriction::stone},
    {"fire", Restriction::fire},
    {"nature", Restriction::nature},
    {"water", Restriction::water},
    {"sharp", Restriction::sharp},
    {"blunt", Restriction::blunt},
    {"cold", Restriction::cold},
    {"poison", Restriction::poison},
    {"lightning", Restriction::lightning},
    {"weapon", Restriction::weapon},
    {"armor", Restriction::armor},
    {"shield", Restriction::shield},
    {"melee", Restriction::melee},
    {"ranged", Restriction::ranged},
    {"thrown", Restriction::thrown}
};


Restrictions ParseRestrictions(const json& data, std::string_view tag)
{
    Restrictions result;
    const auto& restrictionData = Engine::try_get<json>(data, tag);
    if (!restrictionData)
    {
        return result;
    }
    for(auto restriction : *restrictionData)
    {
        auto restictionLabel = restriction.get<std::string_view>();
        result.insert(Engine::from_string(restictionLabel, restrictionMapping));
    }
    return result;
}
Item::Item(const json& data) :
    name(Engine::must_get<std::string_view>(data, "name")),
    restrictions(ParseRestrictions(data, "tags"))
{
    Statistics::Load(data);
    // TODO: load non item stats as bonuses
}

std::string_view Item::Name() const
{
    return name;
}

StatDescriptor Item::Get(Stat::Id id) const
{
    return Statistics::Get(id);
}

StatDefinition Item::definition;

const StatDefinition& Item::Definition() const
{
    return definition;
}


};
