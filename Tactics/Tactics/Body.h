#pragma once
#include <string>
#include <map>
#include "Stats.h"

namespace Game
{
    class Damage;

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
        std::map<Part, Damage> health;
        std::wstring Description(unsigned constitution) const;
        bool Dead(unsigned constitution) const;
        void Hurt(Part::Type location, Damage& damage);
        Stats stats;
    };
    std::wistream& operator>>(std::wistream& s, Body& body);

};
