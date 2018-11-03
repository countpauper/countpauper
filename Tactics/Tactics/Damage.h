#pragma once
#include <string>
#include <map>
#include <array>
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
            Spirit,
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
        Damage();
        Damage(int sharp, int crush, int burn, int disease, int spirit);
        std::wstring StateDescription() const;
        std::wstring ActionDescription() const;
        int StatPenalty() const;
        bool Disabled() const;
        bool Hurt() const;

        void Set(Wound::Type type, int value) { damage[unsigned(type)] = value; }
        void SetSharp(const int& v) { Set(Wound::Type::Sharp, v);  }
        void SetCrush(const int& v)  { Set(Wound::Type::Sharp, v); }
        void SetBurn(const int& v)  { Set(Wound::Type::Sharp, v); }
        void SetDisease(const int& v) { Set(Wound::Type::Sharp, v); }
        void SetSpirit(const int& v)  { Set(Wound::Type::Sharp, v); }

        std::wstring CsvString() const;

        Damage operator+(const Damage& other) const;
        Damage operator-(const Damage& other) const;
        Damage& operator-=(const Damage& other);
        Damage& operator+=(const Damage& other);

    protected:
        friend std::wostream& operator<<(std::wostream& s, const Damage& damage);
        friend std::wistream& operator>>(std::wistream& s, Damage& damage);

        std::pair<Wound::Type, int> FindWorst() const;
        std::array<int, 5> damage;
    };
    std::wistream& operator>>(std::wistream& s, Damage& damage);
    std::wostream& operator<<(std::wostream& s, const Damage& damage);
};
