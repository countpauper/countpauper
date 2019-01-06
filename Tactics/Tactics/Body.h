#pragma once
#include <string>
#include <vector>
#include <set>
#include "Stats.h"
#include "Damage.h"
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
        private:
            friend std::wistream& operator>>(std::wistream& s, Body::Part& part);
            std::wstring name;
            std::set<Anatomy> anatomy;
            Slot slot;
            Attributes attributes;
            Stats::Score score;
            Damage health;
        };
        std::wstring Description() const;
        bool Dead() const;
        bool Hurt(const Anatomy& location, const Damage& damage);
        Slot FindSlot(const Anatomy& target) const;
        const Part* Get(const Anatomy& target) const;
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
