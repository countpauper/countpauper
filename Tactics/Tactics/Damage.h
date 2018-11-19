#pragma once
#include <string>
#include <map>
#include <set>
#include "Score.h"

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
            Blunt,
            Burn,
            Disease,
            Spirit,
        };

        typedef std::map<Wound::Type, std::map<Pain, Wound>> Table;
        static const Table table;
        static const Wound& find(Type type, Pain value);

        std::wstring stateDescription;
        std::wstring actionDescription;
        int penalty;

        static const std::set<Type> Types;
    };

    class Damage
    {
    public:
        Damage();
        Damage(Wound::Type type, const Score& pain);
        std::wstring StateDescription() const;
        std::wstring ActionDescription() const;
        int StatPenalty() const;
        bool Disabled() const;
        bool Hurt() const;

        void Set(Wound::Type type, int value) { if (value) damage[type] = Score(L"",value); }
        void SetSharp(const int& v) { Set(Wound::Type::Sharp, v);  }
        void SetBlunt(const int& v)  { Set(Wound::Type::Blunt, v); }
        void SetBurn(const int& v)  { Set(Wound::Type::Burn, v); }
        void SetDisease(const int& v) { Set(Wound::Type::Disease, v); }
        void SetSpirit(const int& v)  { Set(Wound::Type::Spirit, v); }

        Score Get(Wound::Type type) const { return (damage.count(type))?damage.at(type):Score(); };
        Score Sharp() const { return Get(Wound::Type::Sharp); }
        Score Blunt() const { return Get(Wound::Type::Blunt); }
        Score Burn() const { return Get(Wound::Type::Burn); }
        Score Disease() const { return Get(Wound::Type::Disease); }
        Score Spirit() const { return Get(Wound::Type::Spirit); }

        std::wstring CsvString() const;

        Damage operator+(const Damage& other) const;
        Damage operator^(const Damage& other) const;
        Damage& operator^=(const Damage& other);
        Damage& operator+=(const Damage& other);
        Damage Wound(const std::wstring& description) const;
    protected:
        friend std::wostream& operator<<(std::wostream& s, const Damage& damage);
        friend std::wistream& operator>>(std::wistream& s, Damage& damage);
        friend Damage operator-(const Damage& damage, const Damage& mitigation);

        std::pair<Wound::Type, unsigned> FindWorst() const;
        std::map<Wound::Type, Score> damage;
    };
    Damage operator-(const Damage& damage, const Damage& mitigation);

    std::wistream& operator>>(std::wistream& s, Damage& damage);
    std::wostream& operator<<(std::wostream& s, const Damage& damage);
};
