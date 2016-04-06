#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "Health.h"

namespace Game
{
    class Game;

    class Requirement
    {
    public:
        Requirement() : strength(0), wisdom(0) {}
        Requirement(int strength, int wisdom) : strength(strength), wisdom(wisdom) {}
        int strength;
        int wisdom;
        Requirement operator+(const Requirement& other) const;
        Requirement operator*(int multiplier) const;
    };

    
    enum class Element
    {
        None = 0,
        Fire,
        Water,
        Nature,
        Stone,
        Air
    };

    enum class Statistic
    {
        None = 0,
        Strength,
        Agility,
        Constitution,
        Intelligence,
        Wisdom
    };

    namespace Type
    {
        class Item
        {
        public:
            Item() : frequency(0) {}
            std::wstring name;
            int frequency;  
        };

        class Equipment : public Item
        {
        public:
            Requirement requirement;
        };


        class Material
        {
        public:
            Material() : frequency(0) {}
            std::wstring name;
            int frequency;
            int magic;
            Element element;
            Requirement requirement;
        };

        class Bonus
        {
        public:
            Bonus() : frequency(0), magic(0) {}
            std::wstring prefix, postfix;
            int frequency;
            int magic;
            Requirement requirement;
        };

    
        enum class Covers : unsigned
        {
            Nothing = 0,
            Belly = 1,
            Chest = 2,
            Arms = 4,
            Legs = 8,
            Head = 16,
            Finger = 32,
            Neck = 64,

            Trunk = Belly+Chest,
            Shirt = Trunk +Arms,
            Body = Trunk + Arms + Legs,
            Full = Body + Head,

        };

        class Armor : public Equipment
        {
        public:
            enum Category
            {
                None = 0,
                Cloth = 1<<0,
                Leather = 1<<1,
                Chain = 1<<2,
                Scale = 1<<3,
                Plate = 1<<4,
                Metal = Chain | Scale | Plate,
                BodyArmor = Metal | Leather | Cloth,
                Necklace = 1<<5,
                Ring = 1<<6,
                Bracer = 1<<7,
                Crown = 1<<8,
                Jewelry = Necklace|Ring|Bracer|Crown,
                All = Jewelry | BodyArmor,
            };
            class Material : public Type::Material
            {
            public:
                Material() : category(None) {}
                Damage mitigation;
                Armor::Category category;
                static std::vector<Material> Load(std::wistream& fileName);
            };
            class Bonus : public Type::Bonus
            {
            public:
                Bonus() : category(None) {}
                Armor::Category category;
                Damage mitigation;
                std::wstring skill;
                int skillBonus;
                Statistic stat;
                int statBonus;
                static std::vector<Bonus> Load(std::wistream& fileName);
            };

            Armor() : cover(Covers::Nothing), category(None) {}
            static std::vector<Armor> Load(std::wistream& fileName);            
            unsigned CoverCount() const;
            bool Match(const Material& material) const;
            bool Match(const Bonus& bonus) const;

            Covers cover;
            Category category;
            Damage mitigation;
        };



        class Weapon: public Equipment
        {
        public:
            enum Style
            {
                All = -1,
                Unarmed = 0,
                Blade,
                Blunt,
                Axe,
                Sling,
                Bow,
                Crossbow,
                Gun,
            };
            class Material : public Type::Material
            {
            public:
                enum Category
                {
                    None = 0,
                    Wood =1<<0,
                    Metal = 1<<1,
                    Leather= 1<<2,
                };
                Material() : category(None) {}
                Category category;
                Damage damage;
                static std::vector<Material> Load(std::wistream& fileName);
            };
            class Bonus : public Type::Bonus
            {
            public:
                Bonus() : style(Weapon::All) {}
                Weapon::Style style;
                Damage damage;
                static std::vector<Bonus> Load(std::wistream& fileName);
            };
            Weapon() : style(Unarmed), material(Material::None) {}
            bool Match(const Material& material) const;
            bool Match(const Bonus& bonus) const;

            Style style;
            int hands;
            Material::Category material;
            Damage damage;
            static std::vector<Weapon> Load(std::wistream& fileName);

        };
    }

    class Item
    {
    public:
    };

    class Equipment : public Item
    {
    };

    class Armor : public Equipment
    {
    public:
        Armor(const Game& game, const std::wstring& type, const std::wstring& material, const std::wstring& bonus);
        Armor(const Type::Armor& type, const Type::Armor::Material& material, const Type::Armor::Bonus& bonus);
        std::wstring Name() const;
        Requirement Required() const;
        Damage Mitigation() const;
    private:
        const Type::Armor& type;
        const Type::Armor::Material &material;
        const Type::Armor::Bonus &bonus;
    };

    class Weapon : public Equipment
    {
    public:
        Weapon(const Game& game, const std::wstring& type, const std::wstring& material, const std::wstring& bonus);
        Weapon(const Type::Weapon& type, const Type::Weapon::Material& material, const Type::Weapon::Bonus& bonus);
        std::wstring Name() const;
        Requirement Required() const;
        Damage Damage() const;
    private:
        const Type::Weapon& type;
        const Type::Weapon::Material &material;
        const Type::Weapon::Bonus &bonus;
    };
}

