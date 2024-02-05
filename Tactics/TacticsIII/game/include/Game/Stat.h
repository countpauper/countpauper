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
        Id dependency;
        std::vector<int> perLevel;      // if dependency is not None
        Id multiplier;                  // 1 if None
        int maximum;
};

// Loaded from JSON each Stat is defined by a name and which bonuses it gives to other stats at a certain level.
// This level has to be recursively queried from the character along with all its bonuses
// percentages are in whole %
static std::map<Stat::Id, Stat> stats;

}
