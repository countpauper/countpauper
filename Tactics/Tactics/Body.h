#pragma once
#include <string>
#include <vector>
#include "Stats.h"
#include "Damage.h"

namespace Game
{

    enum class AttackVector
    {
        Head,
        Chest,
        Belly,
        Arms,
        Legs,
    };
    class Body
    {
    public:
        class Part
        {
        public:
            bool IsVital() const;
            std::wstring Description(Stats::Score constitution) const;
            bool operator<(const Part& other) const;
            bool Match(AttackVector target) const;
            Stats::Score Score(Attribute attribute) const;
            bool Disabled(Stats::Score constitution) const;
            void Hurt(const Damage& damage);
            bool IsHurt() const;
        private:
            friend std::wistream& operator>>(std::wistream& s, Body::Part& part);
            std::wstring name;
            AttackVector location;
            Attribute attribute;
            Stats::Score score;
            Damage health;
        };
        std::wstring Description(unsigned constitution) const;
        bool Dead(unsigned constitution) const;
        void Hurt(AttackVector location, Damage& damage);
        Stats::Score Strength() const;
        Stats::Score Agility() const;
        Stats::Score Constitution() const;
        Stats::Score Intelligence() const;
        Stats::Score Wisdom() const;
    private:
        friend std::wistream& operator>>(std::wistream& s, Body& body);
        std::vector<Part> parts;
    };
    std::wistream& operator>>(std::wistream& s, Body::Part& part);
    std::wistream& operator>>(std::wistream& s, Body& body);

};
