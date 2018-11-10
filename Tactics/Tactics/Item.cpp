#include "stdafx.h"
#include "Item.h"
#include "CSV.h"
#include "Game.h"

namespace Game
{
    namespace Type
    {
        std::map<std::wstring, Armor::Category> armorCategories(
        {
            { L"Cloth", Armor::Category::Cloth },
            { L"Leather", Armor::Category::Leather },
            { L"Chain", Armor::Category::Chain },
            { L"Scale", Armor::Category::Scale },
            { L"Plate", Armor::Category::Plate },
            { L"Metal", Armor::Category::Metal },
            { L"Armor", Armor::Category::BodyArmor },
            { L"Necklace", Armor::Category::Necklace },
            { L"Ring", Armor::Category::Ring },
            { L"Bracer", Armor::Category::Bracer },
            { L"Crown", Armor::Category::Crown },
            { L"Jewelry", Armor::Category::Jewelry },
            { L"All", Armor::Category::All }
        });

        std::map<std::wstring, Element> elementMap(
        {
            { L"None", Element::None },
            { L"Stone", Element::Stone },
            { L"Water", Element::Water },
            { L"Fire", Element::Fire },
            { L"Air", Element::Air },
            { L"Nature", Element::Nature },
        });

        std::map<std::wstring, Covers> coverMap(
        {
            { L"Nothing", Covers::Nothing },
            { L"Belly", Covers::Belly },
            { L"Chest", Covers::Chest },
            { L"Arms", Covers::Arms },
            { L"Legs", Covers::Legs },
            { L"Head", Covers::Head },
            { L"Finger", Covers::Finger },
            { L"Neck", Covers::Neck },

            { L"Trunk", Covers::Trunk },
            { L"Shirt", Covers::Shirt },
            { L"Body", Covers::Body },
            { L"Full", Covers::Full },
        });

        std::map<std::wstring, Statistic> statisticMap(
        {
            { L"", Statistic::None },
            { L"Strength", Statistic::Strength },
            { L"Agility", Statistic::Agility },
            { L"Constitution", Statistic::Constitution },
            { L"Intelligence", Statistic::Intelligence },
            { L"Wisdom", Statistic::Wisdom }
        });

        Engine::Adapter::Integer<Requirement> reqstr(&Requirement::strength);
        Engine::Adapter::Integer<Requirement> reqwis(&Requirement::wisdom);
        std::vector<Engine::Adapter::Interface<Requirement>*> requirementAdapters({ &reqstr, &reqwis });

        Engine::Adapter::Setter<Damage, int> sharp(&Damage::SetSharp);
        Engine::Adapter::Setter<Damage, int> blunt(&Damage::SetBlunt);
        Engine::Adapter::Setter<Damage, int> burn(&Damage::SetBurn);
        Engine::Adapter::Setter<Damage, int> disease(&Damage::SetDisease);
        Engine::Adapter::Setter<Damage, int> spirit(&Damage::SetSpirit);
        std::vector<Engine::Adapter::Interface<Damage>*> damageAdapters({ &sharp, &blunt, &burn, &disease, &spirit });

        std::vector<Armor> Armor::Load(std::wistream& file)
        {
            Engine::Adapter::String<Armor> name(&Armor::name);
            Engine::Adapter::Integer<Armor> frequency(&Armor::frequency);
            Engine::Adapter::Struct<Armor, Requirement> requirement(&Armor::requirement, requirementAdapters);
            Engine::Adapter::Enumeration<Armor, Covers> covers(&Armor::cover, coverMap);
            Engine::Adapter::Enumeration<Armor, Armor::Category> category(&Armor::category, armorCategories);
            Engine::Adapter::Struct<Armor, Damage> mitigation(&Armor::mitigation, damageAdapters);

            Engine::CSV<Armor> csv(file, { &name, &frequency, &requirement, &covers, &category, &mitigation });
            return csv.Read();
        }

        std::vector<Armor::Material> Armor::Material::Load(std::wistream& file)
        {
            Engine::Adapter::String<Armor::Material> name(&Armor::Material::name);
            Engine::Adapter::Integer<Armor::Material> frequency(&Armor::Material::frequency);
            Engine::Adapter::Enumeration<Armor::Material, Armor::Category> category(&Armor::Material::category, armorCategories);
            Engine::Adapter::Integer<Armor::Material> magic(&Armor::Material::magic);
            Engine::Adapter::Enumeration<Armor::Material, Element> element(&Armor::Material::element, elementMap);
            Engine::Adapter::Struct<Armor::Material, Requirement> requirement(&Armor::Material::requirement, requirementAdapters);
            Engine::Adapter::Struct<Armor::Material, Damage> mitigation(&Armor::Material::mitigation, damageAdapters);

            Engine::CSV<Armor::Material> csv(file, { &name, &frequency, &category, &magic, &element, &requirement, &mitigation });
            return csv.Read();
        }


        std::vector<Armor::Bonus> Armor::Bonus::Load(std::wistream& file)
        {
            Engine::Adapter::String<Armor::Bonus> prefix(&Armor::Bonus::prefix);
            Engine::Adapter::String<Armor::Bonus> postfix(&Armor::Bonus::postfix);
            Engine::Adapter::Integer<Armor::Bonus> frequency(&Armor::Bonus::frequency);
            Engine::Adapter::Integer<Armor::Bonus> magic(&Armor::Bonus::magic);
            Engine::Adapter::Struct<Armor::Bonus, Requirement> requirement(&Armor::Bonus::requirement, requirementAdapters);
            Engine::Adapter::Enumeration<Armor::Bonus, Armor::Category> category(&Armor::Bonus::category, armorCategories);
            Engine::Adapter::Struct<Armor::Bonus, Damage> mitigation(&Armor::Bonus::mitigation, damageAdapters);
            Engine::Adapter::String<Armor::Bonus> skill(&Armor::Bonus::skill);
            Engine::Adapter::Integer<Armor::Bonus> skillBonus(&Armor::Bonus::skillBonus);
            Engine::Adapter::Enumeration<Armor::Bonus, Statistic> stat(&Armor::Bonus::stat, statisticMap);
            Engine::Adapter::Integer<Armor::Bonus> statBonus(&Armor::Bonus::statBonus);
            Engine::CSV<Armor::Bonus> csv(file, { &prefix, &postfix, &frequency, &magic, &requirement, &category, &mitigation, &skill, &skillBonus, &stat, &statBonus });
            return csv.Read();
        }


        unsigned Armor::CoverCount() const
        {
            unsigned result = 0;
            if (unsigned(cover)&unsigned(Covers::Arms))
                result += 1;
            if (unsigned(cover)&unsigned(Covers::Belly))
                result += 1;
            if (unsigned(cover)&unsigned(Covers::Chest))
                result += 1;
            if (unsigned(cover)&unsigned(Covers::Legs))
                result += 1;
            if (unsigned(cover)&unsigned(Covers::Head))
                result += 1;
            if (unsigned(cover)&unsigned(Covers::Finger))
                result += 1;
            if (unsigned(cover)&unsigned(Covers::Neck))
                result += 1;
            return result;
        }

        bool Armor::Match(const Material& material) const
        {
            return (unsigned(material.category) & unsigned(category)) != 0;
        }
        bool Armor::Match(const Bonus& bonus) const
        {
            return (unsigned(bonus.category) & unsigned(category)) != 0;
        }

        std::map<std::wstring, Weapon::Style > weaponStyles(
        {
            { L"All", Weapon::Style::All },
            { L"Unarmed", Weapon::Style::Unarmed },
            { L"Blade", Weapon::Style::Blade },
            { L"Blunt", Weapon::Style::Blunt },
            { L"Axe", Weapon::Style::Axe },
            { L"Sling", Weapon::Style::Sling },
            { L"Bow", Weapon::Style::Bow },
            { L"Crossbow", Weapon::Style::Crossbow },
            { L"Gun", Weapon::Style::Gun },
        });

        std::map<std::wstring, Weapon::Material::Category > weaponMaterialCategories(
        {
            { L"Metal", Weapon::Material::Metal },
            { L"Wood", Weapon::Material::Wood },
            { L"Leather", Weapon::Material::Leather },
        });

        std::vector<Weapon> Weapon::Load(std::wistream& file)
        {
            Engine::Adapter::String<Weapon> name(&Weapon::name);
            Engine::Adapter::Integer<Weapon> frequency(&Weapon::frequency);
            Engine::Adapter::Struct<Weapon, Requirement> requirement(&Weapon::requirement, requirementAdapters);
            Engine::Adapter::Enumeration<Weapon, Weapon::Style> style(&Weapon::style, weaponStyles);
            Engine::Adapter::Integer<Weapon> hands(&Weapon::hands);
            Engine::Adapter::Enumeration<Weapon, Weapon::Material::Category> material(&Weapon::material, weaponMaterialCategories);
            Engine::Adapter::Struct<Weapon, Damage> damage(&Weapon::damage, damageAdapters);

            Engine::CSV<Weapon> csv(file, { &name, &frequency, &requirement, &style, &hands, &material, &damage,  });
            return csv.Read();
        }

        std::vector<Weapon::Material> Weapon::Material::Load(std::wistream& file)
        {
            Engine::Adapter::String<Weapon::Material> name(&Weapon::Material::name);
            Engine::Adapter::Integer<Weapon::Material> frequency(&Weapon::Material::frequency);
            Engine::Adapter::Enumeration<Weapon::Material, Weapon::Material::Category> category(&Weapon::Material::category, weaponMaterialCategories);
            Engine::Adapter::Integer<Weapon::Material> magic(&Weapon::Material::magic);
            Engine::Adapter::Enumeration<Weapon::Material, Element> element(&Weapon::Material::element, elementMap);
            Engine::Adapter::Struct<Weapon::Material, Requirement> requirement(&Weapon::Material::requirement, requirementAdapters);
            Engine::Adapter::Struct<Weapon::Material, Damage> damage(&Weapon::Material::damage, damageAdapters);

            Engine::CSV<Weapon::Material> csv(file, { &name, &frequency, &category, &magic, &element, &requirement, &damage });
            return csv.Read();
        }


        std::vector<Weapon::Bonus> Weapon::Bonus::Load(std::wistream& file)
        {
            Engine::Adapter::String<Weapon::Bonus> prefix(&Weapon::Bonus::prefix);
            Engine::Adapter::String<Weapon::Bonus> postfix(&Weapon::Bonus::postfix);
            Engine::Adapter::Integer<Weapon::Bonus> frequency(&Weapon::Bonus::frequency);
            Engine::Adapter::Integer<Weapon::Bonus> magic(&Weapon::Bonus::magic);
            Engine::Adapter::Struct<Weapon::Bonus, Requirement> requirement(&Weapon::Bonus::requirement, requirementAdapters);
            Engine::Adapter::Enumeration<Weapon::Bonus, Weapon::Style> style(&Weapon::Bonus::style, weaponStyles);
            Engine::Adapter::Struct<Weapon::Bonus, Damage> damage(&Weapon::Bonus::damage, damageAdapters);
            Engine::CSV<Weapon::Bonus> csv(file, { &prefix, &postfix, &frequency, &magic, &requirement, &style, &damage });
            return csv.Read();
        }
        bool Weapon::Match(const Material& material) const
        {
            return (unsigned(material.category) & unsigned(this->material)) != 0;
        }
        bool Weapon::Match(const Bonus& bonus) const
        {
            return bonus.style == All || bonus.style == this->style;
        }


    }

    Requirement Requirement::operator+(const Requirement& other) const
    {
        return Requirement(strength + other.strength, wisdom + other.wisdom);
    }

    Requirement Requirement::operator*(int multiplier) const
    {
        return Requirement(strength + multiplier, wisdom * multiplier);
    }

    Armor::Armor(const Game& game, const std::wstring& type, const std::wstring& material, const std::wstring& bonus) :
        type(game.FindArmor(type)),
        material(game.FindArmorMaterial(material, this->type)),
        bonus(game.FindArmorBonus(bonus, this->type))
    {

    }
    Armor::Armor(const Type::Armor& type, const Type::Armor::Material& material, const Type::Armor::Bonus& bonus) :
        type(type),
        material(material),
        bonus(bonus)
    {
    }

    std::wstring Armor::Name() const
    {
        std::wstring name;
        if (!bonus.prefix.empty())
            name = bonus.prefix + L" ";
        if (!material.name.empty())
            name += material.name + L" ";
        name += type.name;
        if (!bonus.postfix.empty())
            name += L" " + bonus.postfix;
        return name;
    }
    Requirement Armor::Required() const
    {
        return type.requirement + bonus.requirement + material.requirement * type.CoverCount();
    }

    Damage Armor::Mitigation() const
    {
        return type.mitigation + material.mitigation + bonus.mitigation;
    }
    Weapon::Weapon(const Game& game, const std::wstring& type, const std::wstring& material, const std::wstring& bonus) :
        type(game.FindWeapon(type)),
        material(game.FindWeaponMaterial(material, this->type)),
        bonus(game.FindWeaponBonus(bonus, this->type))
    {
    }
    Weapon::Weapon(const Type::Weapon& type, const Type::Weapon::Material& material, const Type::Weapon::Bonus& bonus) :
        type(type),
        material(material),
        bonus(bonus)
    {
    }

    std::wstring Weapon::Name() const
    {
        std::wstring name;
        if (!bonus.prefix.empty())
            name = bonus.prefix + L" ";
        if (!material.name.empty())
            name += material.name + L" ";
        name += type.name;
        if (!bonus.postfix.empty())
            name += L" " + bonus.postfix;
        return name;
    }


    Requirement Weapon::Required() const
    {
        return type.requirement + bonus.requirement + material.requirement;
    }

    Damage Weapon::Damage() const
    {
        return type.damage + material.damage + bonus.damage;
    }

}