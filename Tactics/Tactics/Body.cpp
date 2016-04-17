#include "stdafx.h"
#include "Body.h"
#include "Damage.h"
#include <iostream>
#include <numeric>

namespace Game
{

    bool Body::Part::operator<(const Part& other) const
    {
        return name < other.name;
    }
    bool Body::Part::Match(AttackVector match) const
    {
        return location.Match(match);
    }

    bool Body::Part::Disabled(Stats::Score constitution) const
    {
        return health.Disabled(constitution);
    }
    void Body::Part::Hurt(const Damage& damage)
    {
        health += damage;
    }
    bool Body::Part::IsVital() const
    {
        return true;
    }

    std::wstring Body::Part::Description(Stats::Score constitution) const
    {
        return name + L"= " + health.Description(constitution);
    }

    Stats::Score Body::Part::Score(Attribute attribute) const
    {
        if (attribute == this->attribute)
            return score;
        else
            return 0;
    }

    bool Body::Part::IsHurt() const
    {
        return health.Hurt();
    }

    std::wstring Body::Description(unsigned constitution) const
    {
        std::wstring result;
        bool fine = true;
        for (auto& part : parts)
        {
            if (part.IsHurt())
            {
                result += part.Description(constitution);
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
        for (auto& part : parts)
        {
            if (part.IsVital() && part.Disabled(constitution))
                return true;
        }
        return false;
    }
    bool Body::Hurt(AttackVector location, Damage& damage)
    {
        for (auto& part : parts)
        {
            if (part.Match(location))
            {
                part.Hurt(damage); 
                return true;
            }
        }
        return false;
    }

    Stats::Score Body::Strength() const
    {
        return std::accumulate(parts.begin(), parts.end(), 0, [](const int& v, const Part& part) { return v + part.Score(Attribute::Strength); });
    }

    Stats::Score Body::Agility() const
    {
        return std::accumulate(parts.begin(), parts.end(), 0, [](const int& v, const Part& part) { return v + part.Score(Attribute::Agility); });
    }

    Stats::Score Body::Constitution() const
    {
        return std::accumulate(parts.begin(), parts.end(), 0, [](const int& v, const Part& part) { return v + part.Score(Attribute::Constitution); });
    }
    
    Stats::Score Body::Intelligence() const
    {
        return std::accumulate(parts.begin(), parts.end(), 0, [](const int& v, const Part& part) { return v + part.Score(Attribute::Intelligence); });
    }
    
    Stats::Score Body::Wisdom() const
    {
        return std::accumulate(parts.begin(), parts.end(), 0, [](const int& v, const Part& part) { return v + part.Score(Attribute::Wisdom); });
    }

    std::wistream& operator>>(std::wistream& s, Body::Part& part)
    {
        std::wstring attribute, location;
        s >> part.name >> part.location >> part.attribute >> part.score >> part.health;
        return s;
    }

    std::wistream& operator>>(std::wistream& s, Body& body)
    {
        unsigned parts;
        s >> parts;
        body.parts.resize(parts);
        for (unsigned i = 0; i < parts; ++i)
        {
            s >> body.parts[i];
        }
        return s;
    }

}
