#pragma once
#include <string>
#include <vector>
#include <set>
#include "Game/Stats.h"
#include "Game/Damage.h"
#include "Game/Anatomy.h"
#include "Score.h"
#include "Game/Slot.h"

namespace Game
{
    class Body
    {
    public:
        class Part
        {
        public:
            Part();
            bool IsVital() const;
            std::wstring Name() const;
            std::wstring Description() const;
            bool operator<(const Part& other) const;
            bool Match(Anatomy target) const;
            Score Score(Attribute attribute) const;
            bool Disabled() const;
            void Hurt(const Damage& damage);
            bool IsHurt() const;
            Slot GetSlot() const;
            unsigned Height() const;
        private:
            friend std::wistream& operator>>(std::wistream& s, Body::Part& part);
            std::wstring name;
            Anatomy anatomy;
            Slot slot;
            Attributes attributes;
            Stats::Score score;
            Damage health;
        };
        std::wstring Description() const;
        bool Dead() const;
        void Hurt(const Part& part, const Damage& damage);
        const Part* Get(const Anatomy& target) const;
        unsigned Height() const;

        Damage InnateDamage() const;
        Score Strength() const;
        Score Agility() const;
        Score Constitution() const;
        Score Intelligence() const;
        Score Wisdom() const;

    private:
        friend std::wistream& operator>>(std::wistream& s, Body& body);
        std::vector<Part> parts;
    };
    std::wistream& operator>>(std::wistream& s, Body::Part& part);
    std::wistream& operator>>(std::wistream& s, Body& body);

};
