#pragma once
#include <string>
#include <map>
#include "Stats.h"

namespace Game
{
    typedef int Pain;
    const Pain MaxPain = 8;
    const Pain ImmunePain = -1;

    struct Wounds
    {
        enum class Type
        {
            Sharp =0,
            Crush,
            Burn,
            Disease,
            Spirit
        };
        std::wstring description;
        int penalty;
        typedef std::map<Wounds::Type, std::map<Pain, Wounds>> Table;
        static const Table table;
    };


    class Damage
    {
    public:
        Damage() : sharp(0), crush(0), burn(0), disease(0), spirit(0) {}
        Damage(int sharp, int crush, int burn, int disease, int spirit) : sharp(sharp), crush(crush), burn(burn), disease(disease), spirit(spirit) {}
        std::wstring Description(unsigned constitution) const;
        bool Disabled(unsigned constitution) const;
        bool Hurt() const;
        int sharp;
        int crush;
        int burn;
        int disease;
        int spirit;
        Damage operator+(const Damage& other) const;
        Damage operator-(const Damage& other) const;
        Damage& operator-=(const Damage& other);
        Damage& operator+=(const Damage& other);
    protected:
        std::pair<Wounds::Type, int> FindWorst() const;
    };
    std::wistream& operator>>(std::wistream& s, Damage& damage);
};
