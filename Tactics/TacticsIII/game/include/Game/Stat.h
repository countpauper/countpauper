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
                None = 0,
                Level,

                Str,
                Lift,
                Damage,
                Athletics,

                Agi,
                Speed,
                AP,
                Dodge,
                Reflex,

                Con,
                Endurance,
                HD,
                Physical,

                Wis,
                Knowledge,
                Mental,
                Attune,

                Int,
                Skill,
                Crit,
                Magic,

                Reach,
                Range,
                Defense,
                Offense,
                Hands,
                Weight,
                Capacity,
                HP,     // this is a tricky one, multiplication

                BluntResist, // is defense? or derived from defense stat and more
                SharpResist, // is defense?
                FireResist,
                ColdResist,
                LightningResist,
                PoisonResist, // includes acid?
        };

        enum Operator
        {
                Nop = 0,
                Add,
                Multiply
        };

        Stat();
        explicit Stat(std::string_view name, std::string_view description=std::string_view());
        Stat(std::string_view name, std::string_view description, Id dependency,
                std::initializer_list<int> table= {}, int multiplier=1);
        Stat(std::string_view name, std::string_view description, Id dependency,
                Operator op, Id operand);
        void Load(const json& j, const class StatDefinition& dependencies);
        StatDescriptor Compute(const class Statted& c) const;
        std::string_view Name() const;
private:
        std::string name;
        std::string description;
        Id dependency = Stat::None;     // If None then it's a primary stat
        std::vector<int> table;         // Only if dependency is not None. If empty then it's linear with dependency
        Operator op = Operator::Nop;
        Id operand = Stat::None;          // 1 if None, with operand add could also be added, then none is 0
        int multiplier = 1;

        Engine::Range<int> range{0, std::numeric_limits<int>::max()};
};



}
