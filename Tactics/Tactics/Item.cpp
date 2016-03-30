#include "stdafx.h"
#include "Item.h"
#include "CSV.h"
#include "Game.h"

namespace Game
{
    namespace Type
    {
        std::map<std::wstring, Armor::Category> materialCategoryMap(
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

        Engine::Adapter::Integer<Damage> sharp(&Damage::sharp);
        Engine::Adapter::Integer<Damage> crush(&Damage::crush);
        Engine::Adapter::Integer<Damage> burn(&Damage::burn);
        Engine::Adapter::Integer<Damage> disease(&Damage::disease);
        Engine::Adapter::Integer<Damage> spirit(&Damage::spirit);
        std::vector<Engine::Adapter::Interface<Damage>*> damageAdapters({ &sharp, &crush, &burn, &disease, &spirit });

        std::vector<Armor::Material> Armor::Material::Load(std::wistream& file)
        {
            Engine::Adapter::String<Armor::Material> name(&Armor::Material::name);
            Engine::Adapter::Integer<Armor::Material> frequency(&Armor::Material::frequency);
            Engine::Adapter::Enumeration<Armor::Material, Armor::Category> category(&Armor::Material::category, materialCategoryMap);
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
            Engine::Adapter::Enumeration<Armor::Bonus, Armor::Category> material(&Armor::Bonus::category, materialCategoryMap);
            Engine::Adapter::Struct<Armor::Bonus, Damage> mitigation(&Armor::Bonus::mitigation, damageAdapters);
            Engine::Adapter::String<Armor::Bonus> skill(&Armor::Bonus::skill);
            Engine::Adapter::Integer<Armor::Bonus> skillBonus(&Armor::Bonus::skillBonus);
            Engine::Adapter::Enumeration<Armor::Bonus, Statistic> stat(&Armor::Bonus::stat, statisticMap);
            Engine::Adapter::Integer<Armor::Bonus> statBonus(&Armor::Bonus::statBonus);
            Engine::CSV<Armor::Bonus> csv(file, { &prefix, &postfix, &frequency, &magic, &requirement, &material, &mitigation, &skill, &skillBonus, &stat, &statBonus });
            return csv.Read();
        }

        std::vector<Armor> Armor::Load(std::wistream& file)
        {
            Engine::Adapter::String<Armor> name(&Armor::name);
            Engine::Adapter::Integer<Armor> frequency(&Armor::frequency);
            Engine::Adapter::Struct<Armor, Requirement> requirement(&Armor::requirement, requirementAdapters);
            Engine::Adapter::Enumeration<Armor, Covers> covers(&Armor::cover, coverMap);
            Engine::Adapter::Enumeration<Armor, Armor::Category> material(&Armor::category, materialCategoryMap);
            Engine::Adapter::Struct<Armor, Damage> mitigation(&Armor::mitigation, damageAdapters);

            Engine::CSV<Armor> csv(file, { &name, &frequency, &requirement, &covers, &material, &mitigation });
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

    Requirement Weapon::Required() const
    {
        return type.requirement + bonus.requirement + material.requirement;
    }

    std::wistream& operator>>(std::wistream& s, Weapon& weapon)
    {
        Game& game = *static_cast<Game*>(s.pword(1));
        return s;
    }

}