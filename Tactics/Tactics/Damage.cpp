#include "stdafx.h"
#include "Damage.h"
#include <sstream>

namespace Game
{
    const Wound::Table Wound::table = 
    {
        { Wound::Type::Crush, {
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
            { 0, { L"Healthy", L"Slip", 0 } },
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

    const Wound& Wound::find(Wound::Type type, Pain severity)
    {
        const auto& table = Wound::table.at(type);
        auto it = table.lower_bound(severity);
        if (it == table.end())
            return table.rbegin()->second;
        else
            return it->second;
    }


    std::wstring Damage::StateDescription() const
    {
        auto worst = FindWorst();
        std::wstringstream ss;
        auto wound = Wound::find(worst.first, worst.second);
        return wound.description;
    }

    std::wstring Damage::ActionDescription() const
    {
        auto worst = FindWorst();
        std::wstringstream ss;
        auto wound = Wound::find(worst.first, worst.second);
        return wound.action;
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
