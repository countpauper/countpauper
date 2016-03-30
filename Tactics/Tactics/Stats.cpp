#include "stdafx.h"
#include "Stats.h"
#include <iostream>
#include <sstream>

namespace Game
{
    Stats::Stats() :
        strength(0),
        agility(0),
        constitution(0),
        intelligence(0),
        wisdom(0)
    {
    }

    std::wstring Stats::Description() const
    {
        std::wstringstream ss;
        ss << "Str: " << strength;
        ss << " Agi: " << agility;
        ss << " Con: " << constitution;
        ss << " Int: " << intelligence;
        ss << " Wis: " << wisdom;
        return ss.str();
    }

    std::wistream& operator>>(std::wistream& s, Stats& stats)
    {
        s >> stats.strength >> stats.agility >> stats.constitution >> stats.intelligence >> stats.wisdom;
        return s;
    }
}
