#include "stdafx.h"
#include "Stats.h"
#include <iostream>
#include <sstream>

namespace Game
{

    std::wistream& operator>>(std::wistream& s, Attribute& attribute)
    {
        std::wstring label;
        s >> label;
        const static std::map<std::wstring, Attribute> attributeMap({
            { L"Str", Attribute::Strength },
            { L"Agi", Attribute::Agility },
            { L"Con", Attribute::Constitution },
            { L"Int", Attribute::Intelligence },
            { L"Wis", Attribute::Wisdom },
        });
        attribute = attributeMap.at(label);
        return s;
    }
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
