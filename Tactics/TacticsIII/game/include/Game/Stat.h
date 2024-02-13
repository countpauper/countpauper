#pragma once
#include <string>
#include <map>
#include <vector>
#include <limits>
#include <initializer_list>
#include "Game/Json.h"
#include "Game/StatDescriptor.h"
#include "Engine/Range.h"

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
                hp,     // this is a tricky one, multiplication
                reach,
                range,
                hands,
                weight,
                carry,

                blunt_resist, // is defense? or derived from defense stat and more
                sharp_resist, // is defense?
                fire_resist,
                cold_resist,
                lightning_resist,
                poison_resist, // includes acid?
        };

        enum Operator
        {
                nop = 0,
                add,
                multiply
        };

        Stat();
        explicit Stat(std::string_view name, std::string_view description=std::string_view());
        Stat(std::string_view name, std::string_view description, Id dependency,
                std::initializer_list<int> table= {}, int multiplier=1);
        Stat(std::string_view name, std::string_view description, Id dependency,
                Operator op, Id operand);
        Stat(std::string_view name, const json& j, const class StatDefinition& dependencies);
        StatDescriptor Compute(const class Statted& c) const;
        std::string_view Name() const;
private:
        std::string name;
        std::string description;
        Id dependency = Stat::none;     // If None then it's a primary stat
        std::vector<int> table;         // Only if dependency is not None. If empty then it's linear with dependency
        Operator op = Operator::nop;
        Id operand = Stat::none;          // 1 if None, with operand add could also be added, then none is 0
        int multiplier = 1;

        Engine::Range<int> limit{0, std::numeric_limits<int>::max()};
};



}
