#pragma once
#include <string>
#include <map>
#include <vector>
#include <limits>
#include <span>
#include "File/Json.h"
#include "Game/Computation.h"
#include "Game/Restriction.h"
#include "Utility/Range.h"
#include "Game/Operator.h"

namespace Game
{
class Stat
{
public:
        enum Id
        {
                none = 0,
                level,

                str,
                lift,
                damage,
                athletics,

                agi,
                speed,
                ap,
                dodge,
                reflex,

                con,
                endurance,
                health,
                physical,

                wis,
                knowledge,
                mental,
                attune,
                focus,

                intel,
                technique,
                crit,
                magic,

                defense,
                offense,
                hp,
                jump,
                block,

                reach,
                range,
                hold,
                hands,
                weight,
                wield,
                enchantment,
                price,
                rarity,

                blunt_resist, // is defense? or derived from defense stat and more
                sharp_resist, // is defense?
                heat_resist,
                cold_resist,
                lightning_resist,
                poison_resist, // includes acid

                blunt_damage,
                sharp_damage,
                heat_damage,
                cold_damage,
                lightning_damage,
                poison_damage
        };


    Stat();
    ~Stat();
    explicit Stat(std::string_view name, std::string_view description=std::string_view(), const Restrictions& restrict={});
    Stat(std::string_view name, std::string_view description, Id dependency,
            std::span<int> table= {}, int multiplier=1, const Restrictions& restrict={});
    Stat(std::string_view name, std::string_view description, Id dependency,
            Operator op, Id operand, const Restrictions& restrict={});
    Stat(std::string_view name, const json& j, const class StatDefinition& dependencies);
    Computation Compute(const class Statted& c, const class Boni* extraBoni=nullptr, const Restrictions& restricted={}) const;
    std::string_view Name() const;
    std::string_view Description() const;
    Engine::Range<int> Limit() const;
    bool IsPrimary() const;
    static Stat::Id Identify(std::string_view name);
    static std::string_view Name(Stat::Id);
    static std::map<Stat::Id, int> Deserialize(const json& statData);
    static json Serialize(const std::map<Stat::Id, int>& stats);
private:
    static Stat::Id TryIdentify(std::string_view name);

    std::string name;
    std::string description;
    Restrictions resricted;
    Id dependency = Stat::none;     // If None then it's a primary stat
    std::vector<int> table;         // Only if dependency is not None. If empty then it's linear with dependency
    Operator op = Operator::nop;
    Id operand = Stat::none;          // 1 if None, with operand add could also be added, then none is 0
    int multiplier = 1;
    Engine::Range<int> limit{0, std::numeric_limits<int>::max()};

    static std::map<Stat::Id, std::string_view> statNames;
};



}
