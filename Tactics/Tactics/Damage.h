#pragma once
#include <string>
#include <map>
#include "Stats.h"

namespace Game
{
    typedef int Pain;
    const Pain MaxPain = 15;
    const Pain ImmunePain = -1;

    struct Wound
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
        std::wstring action;
        int penalty;
        typedef std::map<Wound::Type, std::map<Pain, Wound>> Table;
        static const Table table;
        static const Wound& find(Type type, Pain value);
    };


    class Damage
    {
    public:
        Damage() : sharp(0), crush(0), burn(0), disease(0), spirit(0) {}
        Damage(int sharp, int crush, int burn, int disease, int spirit) : sharp(sharp), crush(crush), burn(burn), disease(disease), spirit(spirit) {}
        std::wstring StateDescription() const;
        std::wstring ActionDescription() const;
        int StatPenalty() const;
        bool Disabled() const;
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
        std::pair<Wound::Type, int> FindWorst() const;
    };
    std::wistream& operator>>(std::wistream& s, Damage& damage);
};
