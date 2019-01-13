#include "stdafx.h"
#include "Engine/CSV.h"
#include "Game/Score.h"
#include "Game/Direction.h"
#include "Item.h"
#include "Game.h"

namespace Game
{
    namespace Type
    {
        std::map<std::wstring, Armor::Category> armorCategories(
        {
            { L"Cloth", Armor::Category::Cloth },
            { L"Leather", Armor::Category::Leather },
            { L"Metal", Armor::Category::Metal },
            { L"Armor", Armor::Category::BodyArmor },
            { L"Precious", Armor::Category::Precious },
            { L"Gem", Armor::Category::Gem},
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

        // TODO: get the name of the type/modifier or material into the damage score
        // it has to come from the owner of the damage struct, so either the whole damage struct
        // has to be some sort of lambda adapter that captures the pointer/name of the owner
        // or it has to be applied after loading to the set "base" scores
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
            Engine::Adapter::Enumeration<Armor, Slot> covers(&Armor::slot, slotMap);
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


        std::vector<Armor::Modifier> Armor::Modifier::Load(std::wistream& file)
        {
            Engine::Adapter::String<Armor::Modifier> prefix(&Armor::Modifier::prefix);
            Engine::Adapter::String<Armor::Modifier> postfix(&Armor::Modifier::postfix);
            Engine::Adapter::Integer<Armor::Modifier> frequency(&Armor::Modifier::frequency);
            Engine::Adapter::Integer<Armor::Modifier> magic(&Armor::Modifier::magic);
            Engine::Adapter::Struct<Armor::Modifier, Requirement> requirement(&Armor::Modifier::requirement, requirementAdapters);
            Engine::Adapter::Enumeration<Armor::Modifier, Armor::Category> category(&Armor::Modifier::category, armorCategories);
            Engine::Adapter::Struct<Armor::Modifier, Damage> mitigation(&Armor::Modifier::mitigation, damageAdapters);
            Engine::Adapter::String<Armor::Modifier> skill(&Armor::Modifier::skill);
            Engine::Adapter::Integer<Armor::Modifier> skillBonus(&Armor::Modifier::skillBonus);
            Engine::Adapter::Enumeration<Armor::Modifier, Statistic> stat(&Armor::Modifier::stat, statisticMap);
            Engine::Adapter::Integer<Armor::Modifier> statBonus(&Armor::Modifier::statBonus);
            Engine::CSV<Armor::Modifier> csv(file, { &prefix, &postfix, &frequency, &magic, &requirement, &category, &mitigation, &skill, &skillBonus, &stat, &statBonus });
            return csv.Read();
        }


        unsigned Armor::SlotCount() const
        {
            unsigned result = 0;
            for (unsigned mask = unsigned(slot); mask != 0; mask <<= 1)
            {
                if (mask & 1)
                    result += 1;
            }
            return result;
        }

        bool Armor::Match(const Material& material) const
        {
            return (unsigned(material.category) & unsigned(category)) != 0;
        }
        bool Armor::Match(const Modifier& mod) const
        {
            return (unsigned(mod.category) & unsigned(category)) != 0;
        }

        const std::map<std::wstring, Weapon::Style > Weapon::styleMap(
        {
            { L"All", Weapon::Style::All },
            { L"", Weapon::Style::None },
            { L"None", Weapon::Style::None },
            { L"Blade", Weapon::Style::Blade },
            { L"Club", Weapon::Style::Club },
            { L"Axe", Weapon::Style::Axe },
            { L"Sharp", Weapon::Style::Sharp },
            { L"Blunt", Weapon::Style::Blunt },
            { L"Fist", Weapon::Style::Fist },
            { L"Melee", Weapon::Style::Melee },
            { L"Bow", Weapon::Style::Bow },
            { L"Crossbow", Weapon::Style::Crossbow },
            { L"Gun", Weapon::Style::Gun },
            { L"Wand", Weapon::Style::Wand },
            { L"Throwing", Weapon::Style::Throwing },
            { L"Ranged", Weapon::Style::Ranged },
            { L"Shield", Weapon::Style::Shield },
            { L"Artifact", Weapon::Style::Artifact }
        });

        std::map<std::wstring, Weapon::Material::Category > weaponMaterialCategories(
        {
            { L"None", Weapon::Material::None},
            { L"Metal", Weapon::Material::Metal },
            { L"Wood", Weapon::Material::Wood },
            { L"Wood|Metal", Weapon::Material::Category(Weapon::Material::Wood | Weapon::Material::Metal) },
            { L"Metal|Wood", Weapon::Material::Category(Weapon::Material::Metal | Weapon::Material::Wood) },
            { L"Leather", Weapon::Material::Leather },
            { L"Mundane", Weapon::Material::Mundane } ,
            { L"Precious", Weapon::Material::Precious },
            { L"Gem", Weapon::Material::Gem },
            { L"All", Weapon::Material::All }
        });

        std::vector<Weapon> Weapon::Load(std::wistream& file)
        {
            Engine::Adapter::String<Weapon> name(&Weapon::name);
            Engine::Adapter::Integer<Weapon> frequency(&Weapon::frequency);
            Engine::Adapter::Struct<Weapon, Requirement> requirement(&Weapon::requirement, requirementAdapters);
            Engine::Adapter::Enumeration<Weapon, Weapon::Style> style(&Weapon::style, styleMap);
            Engine::Adapter::Integer<Weapon> hands(&Weapon::hands);
            Engine::Adapter::Enumeration<Weapon, Weapon::Material::Category> material(&Weapon::material, weaponMaterialCategories);
            Engine::Adapter::Struct<Weapon, Damage> damage(&Weapon::damage, damageAdapters);
            Engine::Adapter::Integer<Weapon> range(&Weapon::range);
            Engine::Adapter::Integer<Weapon> defense(&Weapon::defense);
            Engine::CSV<Weapon> csv(file, { &name, &frequency, &requirement, &style, &hands, &material, &damage, &range, &defense });
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

        std::vector<Weapon::Modifier> Weapon::Modifier::Load(std::wistream& file)
        {
            Engine::Adapter::String<Weapon::Modifier> prefix(&Weapon::Modifier::prefix);
            Engine::Adapter::String<Weapon::Modifier> postfix(&Weapon::Modifier::postfix);
            Engine::Adapter::Integer<Weapon::Modifier> frequency(&Weapon::Modifier::frequency);
            Engine::Adapter::Integer<Weapon::Modifier> magic(&Weapon::Modifier::magic);
            Engine::Adapter::Struct<Weapon::Modifier, Requirement> requirement(&Weapon::Modifier::requirement, requirementAdapters);
            Engine::Adapter::Enumeration<Weapon::Modifier, Weapon::Style> style(&Weapon::Modifier::style, Weapon::styleMap);
            Engine::Adapter::Enumeration<Weapon::Modifier, Weapon::Material::Category> material(&Weapon::Modifier::material, weaponMaterialCategories);
            Engine::Adapter::Struct<Weapon::Modifier, Damage> damage(&Weapon::Modifier::damage, damageAdapters);
            Engine::CSV<Weapon::Modifier> csv(file, { &prefix, &postfix, &frequency, &magic, &requirement, &style, &material, &damage });
            return csv.Read();
        }

        Weapon::Weapon() : 
            style(None), 
            material(Material::None),
            range(0),
            defense(0)
        {
        }

       
        bool Weapon::Match(const Material& material) const
        {
            return (unsigned(material.category) & unsigned(this->material)) != 0;
        }
        bool Weapon::Match(const Modifier& mod) const
        {
            return mod.style == All || mod.style == this->style;
        }
    }

    Requirement Requirement::operator+(const Requirement& other) const
    {
        return Requirement(strength + other.strength, wisdom + other.wisdom);
    }

    Requirement Requirement::operator*(int multiplier) const
    {
        return Requirement(strength * multiplier, wisdom * multiplier);
    }

    Armor::Armor(const Game& game, const std::wstring& type, const std::wstring& material, const std::wstring& mod) :
        type(game.FindArmor(type)),
        material(game.FindArmorMaterial(material, this->type)),
        modifier(game.FindArmorModifier(mod, this->type))
    {

    }
    Armor::Armor(const Type::Armor& type, const Type::Armor::Material& material, const Type::Armor::Modifier& mod) :
        type(type),
        material(material),
        modifier(mod)
    {
    }

    std::wstring Armor::Name() const
    {
        std::wstring name;
        if (!modifier.prefix.empty())
            name = modifier.prefix + L" ";
        if (!material.name.empty())
            name += material.name + L" ";
        name += type.name;
        if (!modifier.postfix.empty())
            name += L" " + modifier.postfix;
        return name;
    }
    Requirement Armor::Required() const
    {
        return type.requirement + modifier.requirement + material.requirement * type.SlotCount();
    }

    Damage Armor::Mitigation(Slot slot) const
    {
        if (unsigned(type.slot) & unsigned(slot))
        {
            return Mitigation();
        }
        else
        {
            return Damage();
        }
    }

    Damage Armor::Mitigation() const
    {
        return type.mitigation + material.mitigation + modifier.mitigation;
    }

    Weapon::Weapon(const Game& game, const std::wstring& type, const std::wstring& material, const std::wstring& mod) :
        type(game.FindWeapon(type)),
        material(game.FindWeaponMaterial(material, this->type)),
        modifier(game.FindWeaponModifier(mod, this->type))
    {
    }
    Weapon::Weapon(const Type::Weapon& type, const Type::Weapon::Material& material, const Type::Weapon::Modifier& modifier) :
        type(type),
        material(material),
        modifier(modifier)
    {
    }

    std::wstring Weapon::Name() const
    {
        std::wstring name;
        if (!modifier.prefix.empty())
            name = modifier.prefix + L" ";
        if (!material.name.empty())
            name += material.name + L" ";
        name += type.name;
        if (!modifier.postfix.empty())
            name += L" " + modifier.postfix;
        return name;
    }


    Requirement Weapon::Required() const
    {
        return type.requirement + modifier.requirement + material.requirement;
    }

    Damage Weapon::Damage() const
    {
        return type.damage + material.damage + modifier.damage;
    }

    bool Weapon::Match(Type::Weapon::Style style) const
    {
        return (unsigned(style) & unsigned(type.style)) != 0;
    }

    Score Weapon::RangeBonus() const
    {
        return Score(type.name, type.range);
    }

    Score Weapon::DefenseBonus() const
    {
        return Score(type.name, type.defense);
    }
}