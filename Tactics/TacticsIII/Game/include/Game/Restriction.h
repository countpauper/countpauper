#pragma once
#include <set>
#include <nlohmann/json_fwd.hpp>

namespace Game
{

enum class Restriction
{
    none = 0,
    creature = 0x01,
    item = 0x02,
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

class Restrictions
{
public:
    Restrictions() = default;
    Restrictions(std::initializer_list<Restriction> init);
    explicit Restrictions(const nlohmann::json& data);
    bool Match(const Restrictions& tags) const;
    bool Contains(Restriction tag) const;
    bool operator==(const Restrictions& o) const;
    Restrictions& operator|=(Restriction add);
    static Restrictions Parse(const nlohmann::json& data, std::string_view tag);
private:
    unsigned Categories() const;
    std::set<Restriction> elements;
};


}
