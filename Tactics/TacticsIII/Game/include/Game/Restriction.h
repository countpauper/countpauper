#pragma once
#include <set>
#include <nlohmann/json_fwd.hpp>

namespace Game
{

enum class Restriction
{
    none = 0,
    // material
    cloth=0x10,
    leather,
    metal,
    wood,
    // element
    stone=0x20,
    water,
    fire,
    nature,
    air,
    // damage
    sharp=0x30,
    blunt,
    cold,
    heat,
    poison,
    lightning,
    // weapon or armor type
    armor=0x40,
    shield,
    melee,
    ranged,
    thrown,
    unarmed
};

// TODO: this should be a conjunction of disjunctions where items in a category are disjunction
using Restrictions = std::set<Restriction>;

bool Match(const Restrictions& tags, const Restrictions& restrictions);

Restrictions ParseRestrictions(const nlohmann::json& data, std::string_view tag);

}
