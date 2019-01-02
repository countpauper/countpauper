#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "Damage.h"

namespace Game
{
    class Game;
    class Score;

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

        class Modifier
        {
        public:
            Modifier() : frequency(0), magic(0) {}
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
                Cloth = 1 << 0,
                Leather = 1 << 1,
                Metal = 1 << 2, 
                BodyArmor = Metal | Leather | Cloth,
                Precious = 1<<5,
                Gem = 1<<6,
                Jewelry = Precious|Gem,
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
            class Modifier : public Type::Modifier
            {
            public:
                Modifier() : category(None) {}
                Armor::Category category;
                Damage mitigation;
                std::wstring skill;
                int skillBonus;
                Statistic stat;
                int statBonus;
                static std::vector<Modifier> Load(std::wistream& fileName);
            };

            Armor() : cover(Covers::Nothing), category(None) {}
            static std::vector<Armor> Load(std::wistream& fileName);            
            unsigned CoverCount() const;
            bool Match(const Material& material) const;
            bool Match(const Modifier& bonus) const;

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
                None = 0,
                Blade,
                Blunt,
                Axe,
                Fist,
                Bow,
                Crossbow,
                Gun,
                Wand,
                Throwing,
                Shield
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
                    Mundane = Wood | Metal | Leather,
                    Precious = 1 << 3,
                    Gem = 1<<4,
                    All = Wood | Metal | Leather | Precious | Gem
                };
                Material() : category(None) {}
                Category category;
                Damage damage;
                static std::vector<Material> Load(std::wistream& fileName);
            };
            class Modifier : public Type::Modifier
            {
            public:
                Modifier() : style(Weapon::All) {}
                Weapon::Style style;
                Weapon::Material::Category material;
                Damage damage;
                static std::vector<Modifier> Load(std::wistream& fileName);
            };
            Weapon();
            bool Match(const Material& material) const;
            bool Match(const Modifier& mod) const;

            Style style;
            static const std::map<std::wstring, Style> styleMap;
            int hands;
            Material::Category material;
            Damage damage;
            int range; 
            int defense;
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
        Armor(const Game& game, const std::wstring& type, const std::wstring& material, const std::wstring& mod);
        Armor(const Type::Armor& type, const Type::Armor::Material& material, const Type::Armor::Modifier& mod);
        std::wstring Name() const;
        Requirement Required() const;
        Damage Mitigation() const;
    private:
        const Type::Armor& type;
        const Type::Armor::Material &material;
        const Type::Armor::Modifier &modifier;
    };

    class Weapon : public Equipment
    {
    public:
        Weapon(const Game& game, const std::wstring& type, const std::wstring& material, const std::wstring& mod);
        Weapon(const Type::Weapon& type, const Type::Weapon::Material& material, const Type::Weapon::Modifier& mod);
        std::wstring Name() const;
        Requirement Required() const;
        Damage Damage() const;
        bool Match(Type::Weapon::Style style) const;
        Score RangeBonus() const;
        Score DefenseBonus() const;
    private:
        const Type::Weapon& type;
        const Type::Weapon::Material &material;
        const Type::Weapon::Modifier &modifier;
    };
}

