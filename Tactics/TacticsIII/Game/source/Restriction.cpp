#include "Game/Restriction.h"
#include "File/Json.h"
#include "Utility/from_string.h"

namespace Game
{

using json = ::nlohmann::json;
static std::map<std::string_view, Restriction> restrictionMapping {
    {"", Restriction::none},
    {"cloth", Restriction::cloth},
    {"leather", Restriction::leather},
    {"metal", Restriction::metal},
    {"wood", Restriction::wood},
    {"stone", Restriction::stone},
    {"fire", Restriction::fire},
    {"nature", Restriction::nature},
    {"water", Restriction::water},
    {"air", Restriction::air},
    {"sharp", Restriction::sharp},
    {"blunt", Restriction::blunt},
    {"cold", Restriction::cold},
    {"heat", Restriction::heat},
    {"poison", Restriction::poison},
    {"lightning", Restriction::lightning},
    {"armor", Restriction::armor},
    {"shield", Restriction::shield},
    {"melee", Restriction::melee},
    {"ranged", Restriction::ranged},
    {"thrown", Restriction::thrown}
};

unsigned Category(Restriction restriction)
{
    assert((unsigned(restriction)>>8) < sizeof(unsigned)*8);
    return 1<<(unsigned(restriction)>>8);
}

unsigned Categorize(const Restrictions restrictions)
{
    unsigned categories = 0;
    for(auto restriction: restrictions)
    {
        categories |= Category(restriction);
    }
    return categories;
}

bool Match(const Restrictions& tags, const Restrictions& restrictions)
{
    unsigned requiedCategories = Categorize(restrictions);
    unsigned matchedCategories = 0;
    for(auto restriction: restrictions)
    {
        if (tags.count(restriction) != 0)
            matchedCategories |= Category(restriction);
    }
    return requiedCategories == matchedCategories;
}

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

}
