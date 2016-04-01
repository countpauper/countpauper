#include "stdafx.h"
#include "Health.h"
#include <iostream>
#include <sstream>

namespace Game
{
    const Wounds::Table Wounds::table = 
    {
        { Wounds::Type::Crush, {
            { ImmunePain, { L"Immune", 0 } },
            { 0, { L"Healthy", 0 } },
            { 1, { L"Bruised", -1 } },
            { 3, { L"Battered", -2 } },
            { 5, { L"Broken", -3 } },
            { 8, { L"Crushed", -4 } }
        }},
        { Wounds::Type::Sharp, {
            { ImmunePain, { L"Immune", 0 } },
            { 0, { L"Healthy", 0 } },
            { 1, { L"Scratched", -1 } },
            { 3, { L"Bleeding", -2 } },
            { 5, { L"Hemorrhaging", -3 } },
            { 8, { L"Severed", -4 } }
        } },        
        { Wounds::Type::Burn, {
            { ImmunePain, { L"Immune", 0 } },
            { 0, { L"Healthy", 0 } },
            { 1, { L"Singed", -1 } },
            { 3, { L"Scorched", -2 } },
            { 5, { L"Burned", -3 } },
            { 8, { L"Charred", -4 } }
        } },
        { Wounds::Type::Disease, {
            { ImmunePain, { L"Immune", 0 } },
            { 0, { L"Healthy", 0 } },
            { 1, { L"Infected", -1 } },
            { 3, { L"Festering", -2 } },
            { 5, { L"Decaying", -3 } },
            { 8, { L"Necrotic", -4 } }
        } },
        { Wounds::Type::Spirit, {
            { ImmunePain, { L"Immune", 0 } },
            { 0, { L"Normal", 0 } },
            { 1, { L"Trembling", -1 } },
            { 3, { L"Shaking", -2 } },
            { 5, { L"Tremorring", -3 } },
            { 8, { L"Paralized", -4 } }
        } }
    };

    std::wstring Damage::Description() const
    {
        auto worst = FindWorst();
        std::wstringstream ss;
        auto& table = Wounds::table.at(worst.first);
        auto& descriptionIt = table.upper_bound(worst.second);
        descriptionIt--;
        return descriptionIt->second.description;
        return ss.str();
    }

    bool Damage::Hurt() const
    {
        auto worst = FindWorst();
        return worst.second > 0;
    }

    bool Damage::Disabled(unsigned constitution) const
    {
        auto worst = FindWorst();
        return worst.second >= constitution;
    }
    std::pair<Wounds::Type, int> Damage::FindWorst() const
    {
        std::pair<Wounds::Type, int> result(Wounds::Type::Sharp, int(sharp));
        if (int(crush) > result.second)
            result = std::pair<Wounds::Type, int>(Wounds::Type::Crush, int(crush));
        if (int(burn) > result.second)
            result = std::pair<Wounds::Type, int>(Wounds::Type::Burn, int(burn));
        if (int(disease) > result.second)
            result = std::pair<Wounds::Type, int>(Wounds::Type::Disease, int(disease));
        if (int(spirit) > result.second)
            result = std::pair<Wounds::Type, int>(Wounds::Type::Spirit, int(spirit));
        return result;
    }

    std::wistream& operator>>(std::wistream& s, Damage& damage)
    {
        s >> damage.crush >> damage.sharp >> damage.burn >> damage.disease >> damage.spirit;
        return s;
    }

    Damage Damage::operator+(const Damage& other) const
    {
        return Damage(sharp + other.sharp, crush + other.crush, burn + other.burn, disease + other.disease, spirit+other.spirit);
    }

    Damage& Damage::operator-=(const Damage& other)
    {
        if (sharp < other.sharp)
            sharp = 0;
        else
            sharp -= other.sharp;
        if (crush < other.crush)
            crush = 0;
        else
            crush -= other.crush;
        if (burn < other.burn)
            burn = 0;
        else
            burn -= other.burn;
        if (disease < other.disease)
            disease = 0;
        else
            disease -= other.disease;
        if (spirit < other.spirit)
            spirit = 0;
        else
            spirit -= other.spirit;
        return *this;
    }
    Damage& Damage::operator+=(const Damage& other)
    {
        sharp += other.sharp;
        crush += other.crush;
        burn += other.burn;
        disease += other.disease;
        spirit += other.spirit;
        return *this;
    }

    bool Body::Part::operator<(const Part& other) const
    {
        return unsigned(type) < unsigned(other.type);
    }
    bool Body::Part::Match(Type match) const
    {
        return type == match;
    }

    bool Body::Part::IsVital() const
    {
        return type == Head || type == Chest || type==Belly;
    }

    std::wstring Body::Part::Description() const
    {
        const static std::map < Body::Part::Type, std::wstring> description = {
            { Body::Part::Head, L"Head" },
            { Body::Part::Arms, L"Arms" },
            { Body::Part::Chest, L"Chest" },
            { Body::Part::Belly, L"Belly" },
            { Body::Part::Legs, L"Legs" },
        };
        return description.at(type);
    }
    std::wstring Body::Description() const
    {
        std::wstring result;
        bool fine = true;
        for (auto& part : health)
        {
            if (part.second.Hurt())
            {
                result += part.first.Description() + L"=" + part.second.Description();
                fine = false;
            }
        }
        if (fine)
            return L"Healthy";
        else
            return result;
    }
    bool Body::Dead(unsigned constitution) const
    {
        for (auto& part : health)
        {
            if (part.first.IsVital() && part.second.Disabled(constitution))
                return true;
        }
        return false;
    }
    void Body::Hurt(Part::Type location, Damage& damage)
    {
        for (auto& part : health)
        {
            if (part.first.Match(location))
            {
                part.second += damage;
            }
        }
    }
    std::wistream& operator>>(std::wistream& s, Body& body)
    {
        unsigned partMask;
        s >> partMask;
        for (unsigned bit = 0; bit < sizeof(partMask) * 8; ++bit)
        {
            if (partMask & (1 << bit))
            {
                Damage health;
                s >> health;
                body.health[Body::Part(1 << bit)] = health;
            }
        }
        return s;
    }

}
