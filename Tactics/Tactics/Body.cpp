#include "stdafx.h"
#include "Body.h"
#include "Damage.h"
#include <iostream>

namespace Game
{

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
        return type == Head || type == Chest || type == Belly;
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
    std::wstring Body::Description(unsigned constitution) const
    {
        std::wstring result;
        bool fine = true;
        for (auto& part : health)
        {
            if (part.second.Hurt())
            {
                result += part.first.Description() + L"=" + part.second.Description(constitution);
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
        s >> body.stats;
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
