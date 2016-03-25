#pragma once

#include <string>
#include <vector>

namespace Game
{
    namespace Type
    {
        class Item
        {
        public:
            Item() : rarity(0.0) {}
            std::wstring name;
            double rarity;  
        };

        class Requirement
        {
            Requirement() : strength(0), wisdom(0) {}
            int strength;
            int wisdom;
        };

        class Damage
        {
        public:
            Damage() : sharp(0), blunt(0), fire(0), disease(0) {}
            int sharp;
            int blunt;
            int fire;
            int disease;
        };

        class Equipment : public Item
        {
        public:
            Requirement requirement;
        };


        class Material
        {
        public:
            enum Category
            {
                None = 0,
                Cloth = 1,
                Leather = 2,
                Metal = 4,
                Wood = 8,
                Magic = 16
            };

            Material() : rarity(0.0), category(None) {}
            std::wstring name;
            double rarity;
            Category category;
            Requirement requirement;
        };

        class Bonus
        {
        public:
            Bonus() : rarity(0.0) {}
            std::wstring name;
            double rarity;
            Requirement requirement;
        };

        class ArmorMaterial : public Material
        {
        public:
            Damage mitigation;
        };

        class ArmorBonus : public Bonus
        {
        public:
            Damage mitigation;
        };
    
        enum class Covers
        {
            Nothing = 0,
            Chest = 1,
            Arms = 2,
            Legs = 4,
            Head = 8

            Breast = Chest,
            Gloves = Arms,
            Boots = Legs,
            Helmet = Head,
            Shirt = Chest + Arms,
            Body = Chest + Arms + Legs,
            Full = Chest + Arms + Legs + Head,
        };

        class Armor : public Equipment
        {
            Armor() : cover(Covers::Nothing), material(Material::None) {}
            Covers cover;
            Material::Category material;
            Damage mitigation;
        };

        class WeaponBonus : public Bonus
        {
        public:
            Damage bonus;
        };
        class WeaponMaterial : public Material
        {
        public:
            Damage bonus;
        };

        class Weapon: public Equipment
        {
        public:
            enum Style
            {
                Unarmed,
                Blade,
                Blunt,
                Ranged,
            };
            Weapon() : style(Unarmed), twohanded(false), material(Material::None) {}
            Style style;
            bool twohanded;
            Damage base;
            Material::Category material;
        };

        using ArmorMaterials = std::vector <ArmorMaterial> ;
        using ArmorBonuses = std::vector <ArmorBonus> ;
        using Armors = std::vector <Armor> ;
        using WeaponMaterials = std::vector <WeaponMaterial> ;
        using WeaponBonuses = std::vector <WeaponBonus> ;
        using Weapons = std::vector <Weapon> ;
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
        Type::Armor& type;
        Type::ArmorMaterial &material;
        Type::ArmorBonus &bonus;
    };

    class Weapon : public Equipment
    {
        Type::Weapon& type;
        Type::WeaponMaterial &material;
        Type::WeaponBonus &bonus;
    };
}

