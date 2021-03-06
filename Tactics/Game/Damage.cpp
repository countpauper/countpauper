#include "stdafx.h"
#include "Damage.h"
#include "Engine/from_string.h"
#include "Engine/Utils.h"

namespace Game
{
    const Wound::Table Wound::table = 
    {
        { Wound::Type::Blunt, {
            { ImmunePain, { L"Immune", L"Invulnerable", 0 } },
            { 0, { L"Healthy", L"Block", 0 } },
            { 1, { L"Bruised", L"Bruise", 0 } },
            { 3, { L"Battered", L"Batter", -1 } },
            { 6, { L"Crushed", L"Crush", -2 } },
            { 10, { L"Broken", L"Break", -3 } },
            { MaxPain, { L"Shattered", L"Shatter", -20 } }
        } },
        { Wound::Type::Sharp, {
            { ImmunePain, { L"Immune", L"Invulnerable", 0 } },
            { 0, { L"Healthy", L"Deflect", 0 } },
            { 1, { L"Scratched", L"Scratch", 0 } },
            { 3, { L"Grazed", L"Graze", -1 } },
            { 6, { L"Cut", L"Cut", -2 } },
            { 10, { L"Hemorrhaging", L"Rent", -3 } },
            { MaxPain, { L"Severed", L"Sever", -20 } }
        } },        
        { Wound::Type::Burn, {
            { ImmunePain, { L"Immune", L"Invulnerable", 0 } },
            { 0, { L"Healthy", L"Fizzle", 0 } },
            { 1, { L"Singed", L"Singe", 0 } },
            { 3, { L"Scorched", L"Scorch", -1 } },
            { 6, { L"Seared", L"Sear", -2 } },
            { 10, { L"Burned", L"Burn", -3 } },
            { MaxPain, { L"Charred", L"Char", -20 } }
        } },
        { Wound::Type::Disease, {
            { ImmunePain, { L"Immune", L"Invulnerable", 0 } },
            { 0, { L"Healthy", L"Resist", 0 } },
            { 1, { L"Tainted", L"Taint", -1 } },
            { 3, { L"Infected", L"Infect", -2 } },
            { 6, { L"Festering", L"Fester", -3 } },
            { 10, { L"Decaying", L"Decay", -4 } },
            { MaxPain, { L"Disintegrated", L"Disintegrate", -20 } }
        } },
        { Wound::Type::Spirit, {
            { ImmunePain, { L"Immune", L"Invulnerable", 0 } },
            { 0, { L"Healthy", L"Endure", 0 } },
            { 1, { L"Frail", L"Enfeeble", -1 } },
            { 3, { L"Impaired", L"Impairs", -2 } },
            { 6, { L"Crippled", L"Cripples", -3 } },
            { 10, { L"Paralyzed", L"Paralyse", -4 } },
            { MaxPain, { L"Devastated", L"Devastate", -20 } }
        } }
    };

    const std::set<Wound::Type> Wound::Types = { Wound::Type::Sharp, Wound::Type::Blunt, Wound::Type::Burn, Wound::Type::Disease, Wound::Type::Spirit };

    const Wound& Wound::find(Wound::Type type, Pain severity)
    {
        const auto& table = Wound::table.at(type);
        auto it = table.upper_bound(severity);
        --it;
        if (it == table.end())
            return table.rbegin()->second;
        else
            return it->second;
    }

    Damage::Damage() 
    {
    }

    Damage::Damage(Wound::Type type, const Score& pain)
    {
        damage[type] = pain; 
    }


    std::wstring Damage::StateDescription() const
    {
        auto worst = FindWorst();
        auto wound = Wound::find(worst.first, worst.second);
        return wound.stateDescription;
    }

	std::wstring Damage::ActionDescription() const
	{
		if (!Hurt())
			return L"Mitigated";
		else
		{
			auto worst = FindWorst();
			auto wound = Wound::find(worst.first, worst.second);
			return wound.actionDescription + L"(" + damage.at(worst.first).Description() + L")";
		}
	}

    bool Damage::Hurt() const
    {
        auto worst = FindWorst();
        return worst.second > 0;
    }

    int Damage::StatPenalty() const
    {
        auto worst = FindWorst();
        auto wound = Wound::find(worst.first, worst.second);
        return wound.penalty;
    }
    bool Damage::Disabled() const
    {
        auto worst = FindWorst();
        if (worst.second == 0) return false;
        return unsigned(worst.second) >= MaxPain;
    }

    std::pair<Wound::Type, unsigned> Damage::FindWorst() const
    {
        Wound::Type worst = Wound::Type::Sharp;
        unsigned worstPain = 0;
        for (const auto& d: damage)
        {
            auto pain = d.second.Value();
            if (pain > worstPain)
            {
                worstPain = pain;
                worst = d.first;
            }
        }
        return std::make_pair(worst,worstPain);
    }

    Damage Damage::Wound(const std::wstring& description) const
    {
        // flatten score for wound
        Damage result;
        for (const auto& d : damage)
        {
            result += Damage(d.first, Score(description, d.second.Value()));
        }
        return result;
    }

    Damage Damage::Describe(const std::wstring& description) const
    {
        Damage result =
            Damage(Wound::Type::Sharp, Score(description, Sharp().Value())) +
            Damage(Wound::Type::Blunt, Score(description, Blunt().Value())) +
            Damage(Wound::Type::Burn, Score(description, Burn().Value())) +
            Damage(Wound::Type::Disease, Score(description, Disease().Value())) +
            Damage(Wound::Type::Spirit, Score(description, Spirit().Value()));
        return result;
    }


    std::wistream& operator>>(std::wistream& s, Damage& damage)
    {
        std::wstring str;
        s >> str;
        auto elements = Engine::Split(str, L',');
        for (auto woundType : Wound::Types)
        {
            if (!elements.front().empty())
            {
                int number = Engine::from_string<int>(elements.front());
                damage.damage[woundType] = Score(L"", number);
            }
        }
        return s;
    }

    std::wostream& operator<<(std::wostream& s, const Damage& damage)
    {
        for (auto woundType : Wound::Types)
        {
            if (damage.Has(woundType))
                s << damage.Get(woundType).Value();
            s << L",";
        }
        return s;
    }

    std::ostream& operator<<(std::ostream& s, const Damage& damage)
    {
        for (auto woundType : Wound::Types)
        {
            if (damage.Has(woundType))
                s << damage.Get(woundType).Value();
            s << L",";
        }
        return s;
    }

    Damage Damage::operator+(const Damage& other) const
    {
        Damage o(*this);
        o += other;
        return o;
    }

    Damage& Damage::operator^=(const Damage& other) 
    {
        for (auto& d : damage)
        {
            if (d.second.Value()>0)
                d.second += other.Get(d.first);
        }
        return *this;
    }


    Damage Damage::operator^(const Damage& other) const
    {
        Damage o(*this);
        o ^= other;
        return o;
    }


    Damage& Damage::operator+=(const Damage& other)
    {

        for (auto& d : other.damage)
        {
            if (damage.count(d.first))
                damage[d.first] += d.second;
            else
                damage[d.first] = d.second;
        }
        return *this;
    }

    bool Damage::operator==(const Damage& other) const
    {
        return ((Sharp() == other.Sharp()) &&
            (Blunt() == other.Blunt()) &&
            (Burn() == other.Burn()) &&
            (Disease() == other.Disease()) &&
            (Spirit() == other.Spirit()));
    }

    Damage operator-(const Damage& damage, const Damage& mitigation)
    {   // NB: damage mitigation only
        Damage result(damage);
        for (auto& d : result.damage)
        {
            // if there is no damage to mitigation, penalties are not applied
            if (d.second.Value() > 0)
            {
                d.second -= mitigation.damage.at(d.first);
            }
        }
        return result;
    }

}
