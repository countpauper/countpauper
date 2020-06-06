#include "stdafx.h"
#include "Stats.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace Game
{
    const std::map<const std::wstring, Attribute> Attributes::map({
        { L"", Attribute::None },
        { L"Str", Attribute::Strength },
        { L"Strength", Attribute::Strength },
        { L"Agi", Attribute::Agility },
        { L"Agility", Attribute::Agility },
        { L"Con", Attribute::Constitution },
        { L"Constitution", Attribute::Constitution },
        { L"Int", Attribute::Intelligence },
        { L"Intelligence", Attribute::Intelligence },
        { L"Wis", Attribute::Wisdom },
        { L"Wisdom", Attribute::Wisdom }
    });

    std::wstring ToString(Attribute attribute) 
    {
        auto it = std::find_if(Attributes::map.begin(), Attributes::map.end(), [attribute](const decltype(Attributes::map)::value_type& pair)
        {
            return pair.second == attribute;
        });
        if (it == Attributes::map.end())
            return L"";
        else
            return it->first;
    }

    std::wistream& operator>>(std::wistream& s, Attribute& attribute)
    {

        std::wstring text;
        s >> text;
        attribute = Attributes::map.at(text);
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

    Stats::Stats(Score strength, Score agility, Score constitution, Score intelligence, Score wisdom) :
        strength(strength),
        agility(agility),
        constitution(constitution),
        intelligence(intelligence),
        wisdom(wisdom)
    {
    }

    Stats::Stats(Attribute attribute, Score score) :
        Stats()
    {
        (*this)[attribute] = score;
    }

    Stats::Stats(Attributes attributes, Stats::Score score) :
        Stats()
    {
        for (auto attr : attributes)
            (*this)[attr]= score;
    }

    Stats::Score Stats::operator[](Attribute attribute) const
    {
        switch (attribute)
        {
            case Attribute::None:
                return Score();
            case Attribute::Strength:
                return strength;
            case Attribute::Agility:
                return agility;
            case Attribute::Constitution:
                return constitution;
            case Attribute::Intelligence:
                return intelligence;
            case Attribute::Wisdom:
                return wisdom;
            default:
                throw std::runtime_error("Unknown stattribute");
        }
    }

    Stats::Score& Stats::operator[](Attribute attribute)
    {
        switch (attribute)
        {
        case Attribute::Strength:
            return strength;
        case Attribute::Agility:
            return agility;
        case Attribute::Constitution:
            return constitution;
        case Attribute::Intelligence:
            return intelligence;
        case Attribute::Wisdom:
            return wisdom;
        default:
            throw std::runtime_error("Unknown stattribute");
        }
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
