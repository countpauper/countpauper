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

    bool Body::Part::Disabled() const
    {
        return health.Disabled();
    }
    void Body::Part::Hurt(const Damage& damage)
    {
        health += damage;
    }
    bool Body::Part::IsVital() const
    {
        return true;
    }

    std::wstring Body::Part::Description() const
    {
        return name + L"= " + health.StateDescription();
    }

    Score Body::Part::Score(Attribute attribute) const
    {
        if (attributes.count(attribute) != 0)
            return Game::Score(name, score) + Bonus(health.StateDescription(), health.StatPenalty());
        else
            return Game::Score();
    }

    bool Body::Part::IsHurt() const
    {
        return health.Hurt();
    }

    std::wstring Body::Description() const
    {
        std::wstring result;
        bool fine = true;
        for (auto& part : parts)
        {
            if (part.IsHurt())
            {
                result += part.Description();
                fine = false;
            }
        }
        if (fine)
            return L"Healthy";
        else
            return result;
    }
    bool Body::Dead() const
    {
        for (auto& part : parts)
        {
            if (part.IsVital() && part.Disabled())
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

    Score Body::Strength() const
    {
        return std::accumulate(parts.begin(), parts.end(), Score(), [](const Score& s, const Part& part) { return s + part.Score(Attribute::Strength); });
    }

    Score Body::Agility() const
    {
        return std::accumulate(parts.begin(), parts.end(), Score(), [](const Score& s, const Part& part) { return s + part.Score(Attribute::Agility); });
    }

    Score Body::Constitution() const
    {
        return std::accumulate(parts.begin(), parts.end(), Score(), [](const Score& s, const Part& part) { return s + part.Score(Attribute::Constitution); });
    }
    
    Score Body::Intelligence() const
    {
        return std::accumulate(parts.begin(), parts.end(), Score(), [](const Score& s, const Part& part) { return s + part.Score(Attribute::Intelligence); });
    }
    
    Score Body::Wisdom() const
    {
        return std::accumulate(parts.begin(), parts.end(), Score(), [](const  Score& s, const Part& part) { return s + part.Score(Attribute::Wisdom); });
    }

    std::wistream& operator>>(std::wistream& s, Body::Part& part)
    {
        s >> part.name >> part.location >> part.attributes >> part.score >> part.health;
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
