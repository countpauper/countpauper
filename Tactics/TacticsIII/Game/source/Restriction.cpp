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

unsigned Restrictions::Categories() const
{
    unsigned categories = 0;
    for(auto restriction: elements)
    {
        categories |= Category(restriction);
    }
    return categories;
}


Restrictions Restrictions::Parse(const json& data, std::string_view tag)
{
    const auto& restrictionData = Engine::try_get<json>(data, tag);
    if (!restrictionData)
    {
        return Restrictions();
    }
    return Restrictions(*restrictionData);
}

Restrictions::Restrictions(std::initializer_list<Restriction> init) :
    elements(init)
{
}

Restrictions::Restrictions(const json& data)
{
    for(auto restriction : data)
    {
        auto restictionLabel = restriction.get<std::string_view>();
        elements.insert(Engine::from_string(restictionLabel, restrictionMapping));
    }
}

bool Restrictions::Contains(Restriction tag) const
{
    return elements.count(tag)!=0;
}

bool Restrictions::operator==(const Restrictions& o) const
{
    return elements == o.elements;
}

Restrictions& Restrictions::operator|=(Restriction add)
{
    elements.insert(add);
    return *this;
}

bool Restrictions::Match(const Restrictions& tags) const
{
    unsigned requiedCategories = Categories();
    unsigned matchedCategories = 0;
    for(auto restriction: elements)
    {
        if (tags.Contains(restriction) != 0)
            matchedCategories |= Category(restriction);
    }
    return requiedCategories == matchedCategories;
}


}