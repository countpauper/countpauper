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
            Part(std::wstring name, Anatomy anatomy, Slot slot, const Stats& stats, const Damage& health);
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
            Stats stats;
            Damage health;
            const Skill* engagement;
            const Weapon* held;
            const Actor* control;
        };
        std::wstring Description() const;
        bool Dead() const;
        void Hurt(const Part& part, const Damage& damage);
        void Add(const Part& p);
        const Part* Get(const Anatomy& target) const;
        const Part* Get(const std::wstring& name) const;
        Part& operator[](const std::wstring& name);
        Part& operator[](const Part& part);
        unsigned Length() const;
        std::set<const Part*> Grip() const;
        std::map<const Part*, const Weapon*> Wielded() const;

        bool Ready(const Skill& skill) const;
        void Engage(const Skill& skill);
        void Disengage();
        std::set<const Part*> FindAvailable(const Skill& skill) const;
		std::map<const Body::Part*, const Weapon*> UsedLimbs(const Skill& skill) const;
		std::pair<const Body::Part*, const Weapon*> UsedWeapon(const Skill& skill) const;

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
