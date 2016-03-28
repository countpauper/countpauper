#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace Game
{
    namespace Type
    {
        class Item
        {
        public:
            Item() : frequency(0) {}
            std::wstring name;
            int frequency;  
        };

        class Requirement
        {
        public:
            Requirement() : strength(0), wisdom(0) {}
            int strength;
            int wisdom;
        };

        class Damage
        {
        public:
            Damage() : sharp(0), crush(0), fire(0), disease(0) {}
            int sharp;
            int crush;
            int fire;
            int disease;
        };

        class Equipment : public Item
        {
        public:
            Requirement requirement;
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

        class Material
        {
        public:
            enum Category
            {
                None = 0,
                Cloth,
                Leather,
                Metal,
                Wood,
                Precious,
                Gem
            };
            Material() : frequency(0), category(None) {}
            std::wstring name;
            int frequency;
            int magic;
            Element element;
            Category category;
            Requirement requirement;
        };

        class Bonus
        {
        public:
            Bonus() : frequency(0) {}
            std::wstring name;
            int frequency;
            Requirement requirement;
        };

    
        enum class Covers
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
            Body = Trunk + Arms + Legs,
            Full = Body + Head,

        };

        class Armor : public Equipment
        {
        public:
            class Material : public Type::Material
            {
            public:
                Damage mitigation;
                static std::vector<Material> Load(std::wistream& fileName);
            };
            class Bonus : public Type::Bonus
            {
            public:
                Damage mitigation;
            };
            Armor() : cover(Covers::Nothing), material(Type::Material::None) {}
            Covers cover;
            Type::Material::Category material;
            Damage mitigation;
        };



        class Weapon: public Equipment
        {
        public:
            class Material : public Type::Material
            {
            public:
                Damage bonus;
            };
            class Bonus : public Type::Bonus
            {
            public:
                Damage bonus;
            };            
            enum Style
            {
                Unarmed,
                Blade,
                Stave,
                Ranged,
            };
            Weapon() : style(Unarmed), twohanded(false), material(Type::Material::None) {}
            Style style;
            bool twohanded;
            Damage base;
            Type::Material::Category material;
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
        Type::Armor& type;
        Type::Armor::Material &material;
        Type::Armor::Bonus &bonus;
        virtual void Default() = 0;
    };

    class Weapon : public Equipment
    {
        Type::Weapon& type;
        Type::Weapon::Material &material;
        Type::Weapon::Bonus &bonus;
    };

    
}

