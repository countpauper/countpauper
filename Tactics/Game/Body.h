#pragma once
#include <string>
#include <vector>
#include <set>
#include "Stats.h"
#include "Damage.h"
#include "Anatomy.h"
#include "Score.h"
#include "Slot.h"

namespace Game
{
    class Actor;
    class Skill;
    class Weapon;

    class Body
    {
    public:
        class Part
        {
        public:
            Part();
            bool IsVital() const;
            bool Grip() const;
            std::wstring Name() const;
            std::wstring Description() const;
            bool operator<(const Part& other) const;
            bool Match(Anatomy target) const;
            Score AttributeScore(Attribute attribute) const;
            bool Disabled() const;
            void Hurt(const Damage& damage);
            bool IsHurt() const;
            Slot GetSlot() const;
            unsigned Height() const;
            unsigned Length() const;
            bool Contributes(Attribute attribute) const;
            void Engage(const Skill& skill);
            void Disengage();
            void Hold(const Weapon& item);
            void Drop();
            const Weapon* Held() const;
            bool IsAvailable(const Skill& skill) const;
            bool IsUsed(const Skill& skill) const;
        private:
            friend std::wistream& operator>>(std::wistream& s, Body::Part& part);
            std::wstring name;
            Anatomy anatomy;
            Slot slot;
            Attributes attributes;
            Stats::Score score;
            Damage health;
            const Skill* engagement;
            const Weapon* held;
            const Actor* control;
        };
        std::wstring Description() const;
        bool Dead() const;
        void Hurt(const Part& part, const Damage& damage);
        void Disengage();
        std::map<const Part*, const Weapon*> Wielded() const;
        const Part* Get(const Anatomy& target) const;
        const Part* Get(const std::wstring& name) const;
        Part& Get(const Part& part);
        std::set<const Part*> Grip() const;
        bool Ready(const Skill& skill) const;
        void Engage(const Skill& skill);
        std::set<const Part*> FindAvailable(const Skill& skill) const;
        unsigned Length() const;

        Damage InnateDamage() const;
        Score Strength() const;
        Score Agility() const;
        Score Constitution() const;
        Score Intelligence() const;
        Score Wisdom() const;

    private:
        std::set<const Part*> Required(const Skill& skill) const;
        std::set<const Part*> KineticChain(const Part& origin) const;
        std::set<const Body::Part*> AvailableKineticChain(const Skill& skill) const;
        friend std::wistream& operator>>(std::wistream& s, Body& body);
        std::vector<Part> parts;
    };
    std::wistream& operator>>(std::wistream& s, Body::Part& part);
    std::wistream& operator>>(std::wistream& s, Body& body);

};
