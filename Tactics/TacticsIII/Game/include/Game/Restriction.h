#pragma once
#include <set>
#include <nlohmann/json_fwd.hpp>

namespace Game
{

enum class Restriction : unsigned
{
    none = 0,
    creature = 0x01,
    item = 0x02,
    material = 0x03,
    bonus = 0x04,

    // material
    cloth=0x100,
    leather,
    metal,
    wood,
    // element
    stone=0x200,
    water,
    fire,
    nature,
    air,
    // damage
    sharp=0x300,
    blunt,
    cold,
    heat,
    poison,
    lightning,
    // item sub-type
    armor=0x400,
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
    bool empty() const;
    Restriction at(std::size_t index) const;

    bool operator==(const Restrictions& o) const;
    Restrictions& operator|=(Restriction add);
    Restrictions& operator|=(const Restrictions& add);
    Restrictions& operator&=(const Restrictions& b);
    Restrictions& operator^=(Restriction add);
    static Restrictions Parse(const nlohmann::json& data, std::string_view tag);
    static Restrictions material;
private:
    friend Restrictions operator&(const Restrictions& a, const Restrictions& b);
    unsigned Categories() const;
    std::size_t ClearCategory(unsigned category);
    std::set<Restriction> elements;
};

Restrictions operator&(const Restrictions& a, const Restrictions& b);
Restrictions operator|(const Restrictions& a, Restriction b);
Restrictions operator^(const Restrictions& a, Restriction b);


}
