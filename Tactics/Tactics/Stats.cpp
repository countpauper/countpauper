#include "stdafx.h"
#include "Stats.h"
#include <iostream>
#include <sstream>

namespace Game
{

    std::wistream& operator>>(std::wistream& s, Attribute& attribute)
    {
        const static std::map<std::wstring, Attribute> attributeMap({
            { L"Str", Attribute::Strength },
            { L"Agi", Attribute::Agility },
            { L"Con", Attribute::Constitution },
            { L"Int", Attribute::Intelligence },
            { L"Wis", Attribute::Wisdom }
        });

        std::wstring text;
        s >> text;
        attribute = attributeMap.at(text);
        return s;
    }

    const Attributes Attributes::All { Attribute::Strength, Attribute::Agility, Attribute::Constitution, Attribute::Intelligence, Attribute::Wisdom };

    std::wistream& operator>>(std::wistream& s, Attributes& attributes)
    {
        std::wstring text; 
        s >> text;
        if (text.compare(L"All")==0)
        {
            attributes = Attributes::All;
        }
        else if (text.compare(L"None") == 0)
        {
            attributes = {};
        }
        else
        {
            size_t pos = 0, endpos;
            do
            {
                endpos = text.find_first_of('+', pos);
                auto endLabel = endpos == std::string::npos ? text.size() : endpos;
                std::wstring label;
                label = text.substr(pos, endLabel - pos);
                std::wstringstream strStream(label);
                Attribute attribute;
                strStream >> attribute;
                attributes.insert(attribute);
                pos = endpos + 1;
            } while (endpos != std::string::npos);
        }
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
