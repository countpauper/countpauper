#include "Game/Character.h"

namespace Game
{

Character::Character(std::string_view name, const Race& race) :
        name(name),
        race(race),
        primaryStats{
                {Stat::Level, 1},
                {Stat::Str, 2},
                {Stat::Agi, 2},
                {Stat::Con, 2},
                {Stat::Wis, 2},
                {Stat::Int, 2}
        }
{

}

void Level(Stat::Id stat, int amount)
{

}

int Character::Stat(Stat::Id id) const
{
        auto it = primaryStats.find(id);
        if (it!=primaryStats.end())
        {
                // TODO: add race bonus
                return it->second;
        }
        else
        {
                // TODO: FInd singleton stat list and derive
                return 0;
        }
}


}
