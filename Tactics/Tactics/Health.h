#pragma once
#include <string>
#include <map>

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
        std::wstring Description() const;
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


    class Body
    {
    public:
        class Part
        {
        public:
            enum Type : unsigned
            {
                Head = 0,
                Arms = 1 << 0,
                Chest = 1 << 1,
                Belly = 1 << 2,
                Legs = 1 << 4,
                Humanoid = Arms | Chest | Belly | Legs
            };
            Part(unsigned mask) : type(Type(mask)) {}
            Part(Type type) : type(type) {}
            bool Match(Type type) const;
            bool IsVital() const;
            std::wstring Description() const;
            bool operator<(const Part& other) const;
        private:
            Type type;
        };
        std::map<Part,Damage> health;
        std::wstring Description() const;
        bool Dead(unsigned constitution) const;
        void Hurt(Part::Type location, Damage& damage);
    };
    std::wistream& operator>>(std::wistream& s, Body& body);

};
