#include "Game/Unarmed.h"

namespace Game
{

Unarmed::Unarmed()
{
    name="Unarmed";
    stats[Stat::reach] = 1;
    stats[Stat::hold] = 0;
    stats[Stat::offense] = 1;
    stats[Stat::wield] = 0;
    stats[Stat::enchantment] = 0;
    stats[Stat::price] = 0;
    stats[Stat::rarity] = 0;

    tags = Restrictions{Restriction::melee, Restriction::unarmed, Restriction::blunt};
}

}
