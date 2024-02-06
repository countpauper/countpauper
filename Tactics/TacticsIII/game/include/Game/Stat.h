#pragma once
#include <string>
#include <map>
#include <vector>

namespace Game
{


class Stat
{
public:
        enum Id
        {
                None,
                Level,

                Str,
                Carry,
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

        int Bonus(Id stat);
private:
        std::string name;
        std::string description;
        Id dependency = Stat::None;     // If None then it's a primary stat
        std::vector<int> perLevel;      // Only if dependency is not None. If empty then it's linear with dependency
        // TODO operand, multiply or add at least
        Id operand=Stat::None;          // 1 if None, with operand add could also be added, then none is 0
        int multiplier = 1;

        int maximum;
};

// Loaded from JSON each Stat is defined by a name and which bonuses it gives to other stats at a certain level.
// This level has to be recursively queried from the character along with all its bonuses
// percentages are in whole %
static std::map<Stat::Id, Stat> stats;

}
