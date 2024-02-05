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
                Level,

                Str,
                Carry,
                Damage,
                Athletics,

                Agi,
                Speed,
                AP,
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
        };

        int Bonus(Id stat);
private:
        Id id;
        int maximum;
        std::string name;
        std::map<Id, std::vector<int>> bonusPerLevel;
};

// Loaded from JSON each Stat is defined by a name and which bonuses it gives to other stats at a certain level.
// This level has to be recursively queried from the character along with all its bonuses
// percentages are in whole %
std::vector<Stat> stats;

}
