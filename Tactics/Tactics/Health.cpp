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
            { 2, { L"Bruised", -1 } },
            { 3, { L"Battered", -2 } },
            { 4, { L"Battered", -2 } },
            { 5, { L"Broken", -3 } },
            { 6, { L"Broken", -3 } },
            { 7, { L"Broken", -3 } },
            { 8, { L"Crushed", -4 } }
        }},
        { Wounds::Type::Sharp, {
            { ImmunePain, { L"Immune", 0 } },
            { 0, { L"Health", 0 } },
            { 1, { L"Scratched", -1 } },
            { 2, { L"Scratched", -1 } },
            { 3, { L"Bleeding", -2 } },
            { 4, { L"Bleeding", -2   } },
            { 5, { L"Hemorrhaging", -3 } },
            { 6, { L"Hemorrhaging", -3 } },
            { 7, { L"Hemorrhaging", -3 } },
            { 8, { L"Severed", -4 } }
        } },        
        { Wounds::Type::Fire, {
            { ImmunePain, { L"Immune", 0 } },
            { 0, { L"Health", 0 } },
            { 1, { L"Singed", -1 } },
            { 2, { L"Singed", -1 } },
            { 3, { L"Scorched", -2 } },
            { 4, { L"Scorched", -2 } },
            { 5, { L"Burned", -3 } },
            { 6, { L"Burned", -3 } },
            { 7, { L"Burned", -3 } },
            { 8, { L"Charred", -4 } }
        } },
        { Wounds::Type::Disease, {
            { ImmunePain, { L"Immune", 0 } },
            { 0, { L"Health", 0 } },
            { 1, { L"Infected", -1 } },
            { 2, { L"Infected", -1 } },
            { 3, { L"Festering", -2 } },
            { 4, { L"Festering", -2 } },
            { 5, { L"Decaying", -3 } },
            { 6, { L"Decaying", -3 } },
            { 7, { L"Decaying", -3 } },
            { 8, { L"Necrotic", -4 } }
        } }
    };

    std::wstring Health::Description() const
    {
        auto worst = FindWorst();
        std::wstringstream ss;
        return Wounds::table.at(worst.first).at(worst.second).description;
        return ss.str();
    }

    void Health::Cut(Sharp damage)
    {
        if (Pain(sharp)!=ImmunePain)
            sharp = Sharp(int(sharp)+ int(damage));
    }
    bool Health::Dead() const
    {
        auto worst = FindWorst();
        return worst.second >= 8;
    }
    std::pair<Wounds::Type, int> Health::FindWorst() const
    {
        std::pair<Wounds::Type, int> result(Wounds::Type::Crush, int(crush));
        if (int(sharp) > result.second)
            result = std::pair<Wounds::Type, int>(Wounds::Type::Sharp, int(sharp));
        if (int(burn) > result.second)
            result = std::pair<Wounds::Type, int>(Wounds::Type::Fire, int(burn));
        if (int(disease) > result.second)
            result = std::pair<Wounds::Type, int>(Wounds::Type::Disease, int(disease));
        return result;
    }

    std::wistream& operator>>(std::wistream& s, Health& health)
    {
        int crushDamage, sharpDamage, burnDamage, diseaseDamage;
        s >> crushDamage >> sharpDamage >> burnDamage >> diseaseDamage;// int(health.blunt) >> int(health.sharp) >> int(health.burn) >> int(health.disease);
        health.crush = Crush(crushDamage);
        health.sharp = Sharp(sharpDamage);
        health.burn = Fire(burnDamage);
        health.disease = Disease(diseaseDamage);
        return s;
    }
}
