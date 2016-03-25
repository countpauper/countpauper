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
            Material() : rarity(0.0) {}
            std::wstring name;
            double rarity;
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

        class Armor : public Equipment
        {
            Damage mitigation;
        };

        class HelmetMaterial : public Material
        {
        public:
            Damage mitigation;
        };

        class HelmetBonus : public Bonus
        {
        public:
            Damage mitigation;
        };

        class Helmet : public Equipment
        {
        public:
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
            Damage base;
        };

        using WeaponList = std::vector < Weapon > ;

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
    class Helmet : public Equipment
    {
        Type::Helmet& type;
        Type::HelmetMaterial &material;
        Type::HelmetBonus &bonus;
    };

    class Weapon : public Equipment
    {
        Type::Weapon& type;
        Type::WeaponMaterial &material;
        Type::WeaponBonus &bonus;
    };
}

