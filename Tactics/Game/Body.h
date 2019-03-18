#pragma once
#include <string>
#include <vector>
#include <set>
#include "Stats.h"
#include "Damage.h"
#include "Location.h"
#include "Score.h"
#include "Slot.h"

namespace Game
{
    class Actor;
    class Skill;
    class Weapon;

	class Part
	{
	public:
		Part();
		Part(const Part&) = delete;
		Part& operator=(const Part&) = delete;
		Part(Part&& other);

		Part(std::wstring name, Location anatomy, Slot slot, const Stats& stats);
		std::wstring Name() const;
		std::wstring Description() const;
		bool Match(Location target) const;
		Slot GetSlot() const;
		unsigned Height() const;
		unsigned Length() const;
		bool Contributes(Attribute attribute) const;
		Score AttributeScore(Attribute attribute) const;
		bool Grip() const;
		bool IsVital() const;
	private:
		friend std::wistream& operator>>(std::wistream& s, Part& part);
		std::wstring name;
		Location location;
		Slot slot;
		Stats stats;
	};

	class Anatomy
	{
	public:
		void Add(Part&& p);
		const Part* At(const Location& target) const;
		const Part& operator[](const std::wstring& name) const;
		Part& operator[](const std::wstring& name);
		std::set<const Part*> Grip() const;
		unsigned Length() const;
		Damage InnateDamage() const;
		std::set<const Part*> KineticChain(const Part& origin) const;
	private:
		const Part* Get(const std::wstring& name) const;
		std::set<const Part*> Required(const Skill& skill) const;
		friend std::wistream& operator>>(std::wistream& s, Anatomy& anatomy);
		friend class Body;
		std::vector<Part> parts;
	};

		
	class Body
    {
        class Bit
        {
        public:
			Bit(const Part& anatomy, const Damage& health);
			Bit& operator=(const Bit& other);
			bool Match(const Part& anatomy) const;
			std::wstring Description() const;
			Score AttributeScore(Attribute attribute) const;
            bool Disabled() const;
            void Hurt(const Damage& damage);
            bool IsHurt() const;
            void Engage(const Skill& skill);
            void Disengage();
            void Hold(const Weapon& item);
            void Drop();
            const Weapon* Held() const;
            bool IsAvailable(const Skill& skill) const;
			bool IsUsed(const Skill& skill) const;
		private:
			friend class Body;
			const Part& part;
            Damage health;
            const Skill* engagement;
            const Weapon* held;
            const Actor* control;
        };
	public:
		Body();
		Body(const Anatomy& anatomy);
		Body& operator=(const Body& state);

		const Anatomy& Anatomical() const;
		std::wstring Description() const;
		std::map<const Part*, const Weapon*> Wielded() const;

		void Hurt(const Part& part, const Damage& damage);
		bool IsHurt() const;
		bool Dead() const;
		bool Ready(const Skill& skill) const;
        void Engage(const Skill& skill);
        void Disengage();
        std::set<const Part*> FindAvailable(const Skill& skill) const;
		void Grab(const Part& part, const Weapon& item);
		void Drop(const Weapon& item);
		std::map<const Part*, const Weapon*> UsedLimbs(const Skill& skill) const;
		std::pair<const Part*, const Weapon*> UsedWeapon(const Skill& skill) const;


        Score Strength() const;
        Score Agility() const;
        Score Constitution() const;
        Score Intelligence() const;
        Score Wisdom() const;

    private:
		void Initialize();
		const Bit& Find(const Part& part) const;
		Bit& Find(const Part& part);

		std::set<const Part*> AvailableKineticChain(const Skill& skill) const;
		const Anatomy* anatomy;
		std::vector<Bit> bits;
    };

	std::wistream& operator>>(std::wistream& s, Part& part);
    std::wistream& operator>>(std::wistream& s, Anatomy& body);
};
