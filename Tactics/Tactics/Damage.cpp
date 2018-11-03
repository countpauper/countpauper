#include "stdafx.h"
#include "Damage.h"
#include <sstream>

namespace Game
{
    const Wound::Table Wound::table = 
    {
        { Wound::Type::Crush, {
            { ImmunePain, { L"Immune", 0 } },
            { 0, { L"Healthy", 0 } },
            { 1, { L"Bruised", 0 } },
            { 3, { L"Battered", -1 } },
            { 6, { L"Crushed", -2 } },
            { 10, { L"Broken", -3 } },
            { MaxPain, { L"Shattered", -20 } }
        } },
        { Wound::Type::Sharp, {
            { ImmunePain, { L"Immune", 0 } },
            { 0, { L"Healthy", 0 } },
            { 1, { L"Scratched", 0 } },
            { 3, { L"Cut", -1 } },
            { 6, { L"Bleeding", -2 } },
            { 10, { L"Hemorrhaging", -3 } },
            { MaxPain, { L"Severed", -20 } }
        } },        
        { Wound::Type::Burn, {
            { ImmunePain, { L"Immune", 0 } },
            { 0, { L"Healthy", 0 } },
            { 1, { L"Singed", 0 } },
            { 3, { L"Scorched", -1 } },
            { 6, { L"Seared", -2 } },
            { 10, { L"Burned", -3 } },
            { MaxPain, { L"Charred", -20 } }
        } },
        { Wound::Type::Disease, {
            { ImmunePain, { L"Immune", 0 } },
            { 0, { L"Healthy", 0 } },
            { 1, { L"Infected", -1 } },
            { 3, { L"Diseased", -2 } },
            { 6, { L"Festering", -3 } },
            { 10, { L"Decaying", -4 } },
            { MaxPain, { L"Necrotic", -20 } }
        } },
        { Wound::Type::Spirit, {
            { ImmunePain, { L"Immune", 0 } },
            { 0, { L"Normal", 0 } },
            { 1, { L"Trembling", 0 } },
            { 3, { L"Shaking", -1 } },
            { 6, { L"Spasming", -3 } },
            { 10, { L"Tremoring", -4 } },
            { MaxPain, { L"Paralyzed", -20 } }
        } }
    };

    const Wound& Wound::find(Wound::Type type, Pain severity)
    {
        const auto& table = Wound::table.at(type);
        auto it = table.lower_bound(severity);
        if (it == table.end())
            return table.rbegin()->second;
        else
            return it->second;
    }


    std::wstring Damage::Description(unsigned constitution) const
    {
        auto worst = FindWorst();
        std::wstringstream ss;
        auto wound = Wound::find(worst.first, worst.second);
        return wound.description;
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
        if (worst.second <= 0) return false;
        return unsigned(worst.second) >= MaxPain;
    }
    std::pair<Wound::Type, int> Damage::FindWorst() const
    {
        std::pair<Wound::Type, int> result(Wound::Type::Sharp, int(sharp));
        if (int(crush) > result.second)
            result = std::pair<Wound::Type, int>(Wound::Type::Crush, int(crush));
        if (int(burn) > result.second)
            result = std::pair<Wound::Type, int>(Wound::Type::Burn, int(burn));
        if (int(disease) > result.second)
            result = std::pair<Wound::Type, int>(Wound::Type::Disease, int(disease));
        if (int(spirit) > result.second)
            result = std::pair<Wound::Type, int>(Wound::Type::Spirit, int(spirit));
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
    Damage Damage::operator-(const Damage& other) const
    {
        Damage result(*this);
        result -= other;
        return result;
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
}
