#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace Game
{
    class Requirement
    {
    public:
        Requirement() : strength(0), wisdom(0) {}
        Requirement(int strength, int wisdom) : strength(strength), wisdom(wisdom) {}
        int strength;
        int wisdom;
        Requirement operator+(const Requirement& other);
        Requirement operator*(int multiplier);
    };

    class Damage
    {
    public:
        Damage() : sharp(0), crush(0), fire(0), disease(0), spirit(0) {}
        Damage(int sharp, int crush, int fire, int disease, int spirit) : sharp(sharp), crush(crush), fire(fire), disease(disease), spirit(spirit) {}
        int sharp;
        int crush;
        int fire;
        int disease;
        int spirit;
        Damage operator+(const Damage& other);
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
            static std::vector<Armor> Load(std::wistream& fileName);            
            unsigned CoverCount() const;

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
    public:
        Armor(Type::Armor& type, Type::Armor::Material& material);
        std::wstring Name() const;
        Requirement Required() const;
        Damage Mitigation() const;
    private:
        Type::Armor& type;
        Type::Armor::Material &material;
        Type::Armor::Bonus *bonus;
    };

    class Weapon : public Equipment
    {
    public:
        Type::Weapon& type;
        Type::Weapon::Material &material;
        Type::Weapon::Bonus *bonus;
    };

    
}
