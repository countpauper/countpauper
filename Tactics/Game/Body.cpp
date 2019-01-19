#include "stdafx.h"
#include "Body.h"
#include "Damage.h"
#include <iostream>
#include <numeric>

namespace Game
{
    Body::Part::Part() :
        slot(Slot::Nothing)
    {
    }
    bool Body::Part::operator<(const Part& other) const
    {
        return name < other.name;
    }
    bool Body::Part::Match(Anatomy match) const
    {
        return anatomy.Contains(match);
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
        return attributes.count(Attribute::Intelligence) || 
            attributes.count(Attribute::Wisdom);
    }

    std::wstring Body::Part::Name() const
    {
        return name;
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

    Slot Body::Part::GetSlot() const
    {
        return slot;
    }

    unsigned Body::Part::Height() const
    {
        return anatomy.position + anatomy.size;
    }

    std::wstring Body::Description() const
    {
        std::wstring result;
        bool fine = true;
        for (auto& part : parts)
        {
            if (part.IsHurt())
            {
                if (!fine)
                    result += L",";
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

    void Body::Hurt(const Part& part, const Damage& damage)
    {
        auto& ouchiepart = const_cast<Part&>(part);   // quicker than looking up, functionally equivalent
        ouchiepart.Hurt(damage);
    }

    const Body::Part* Body::Get(const Anatomy& location) const
    {
        for (auto& part : parts)
        {
            if (part.Match(location))
            {
                return &part;
            }
        }
        return nullptr;
    }

    unsigned Body::Height() const
    {
        return std::accumulate(parts.begin(), parts.end(), 0U, [](unsigned max, const decltype(parts)::value_type& part)
        {
            return std::max(max, part.Height());
        });
    }

    Damage Body::InnateDamage() const
    {
        // TODO: configure limb used and use its current "strength" for bite/tail attacks  
        return Damage(Wound::Type::Blunt, Score(L"Innate",1));
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
        s >> part.name >> part.anatomy >> part.slot >> part.attributes >> part.score >> part.health;

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
