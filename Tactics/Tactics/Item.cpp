#include "stdafx.h"
#include "Item.h"
#include "CSV.h"


namespace Game
{
    namespace Type
    {
        std::map<std::wstring, Material::Category> materialCategoryMap(
        {
            { L"Cloth", Material::Category::Cloth },
            { L"Leather", Material::Category::Leather },
            { L"Metal", Material::Category::Metal },
            { L"Precious", Material::Category::Precious },
            { L"Gem", Material::Category::Gem },
            { L"Wood", Material::Category::Wood },
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
            { L"Full", Covers::Full},
        });

        Engine::Adapter::Integer<Requirement> reqstr(&Requirement::strength);
        Engine::Adapter::Integer<Requirement> reqwis(&Requirement::wisdom);
        std::vector<Engine::Adapter::Interface<Requirement>*> requirementAdapters({ &reqstr, &reqwis });

        Engine::Adapter::Integer<Damage> sharp(&Damage::sharp);
        Engine::Adapter::Integer<Damage> crush(&Damage::crush);
        Engine::Adapter::Integer<Damage> fire(&Damage::fire);
        Engine::Adapter::Integer<Damage> disease(&Damage::disease);
        Engine::Adapter::Integer<Damage> spirit(&Damage::spirit);
        std::vector<Engine::Adapter::Interface<Damage>*> damageAdapters({ &sharp, &crush, &fire, &disease, &spirit });

        std::vector<Armor::Material> Armor::Material::Load(std::wistream& file)
        {
            Engine::Adapter::String<Armor::Material> name(&Armor::Material::name);
            Engine::Adapter::Integer<Armor::Material> frequency(&Armor::Material::frequency);
            Engine::Adapter::Enumeration<Armor::Material, Material::Category> category(&Armor::Material::category, materialCategoryMap);
            Engine::Adapter::Integer<Armor::Material> magic(&Armor::Material::magic);
            Engine::Adapter::Enumeration<Armor::Material, Element> element(&Armor::Material::element, elementMap);
            Engine::Adapter::Struct<Armor::Material, Requirement> requirement(&Armor::Material::requirement, requirementAdapters);
            Engine::Adapter::Struct<Armor::Material, Damage> mitigation(&Armor::Material::mitigation, damageAdapters);

            Engine::CSV<Armor::Material> csv(file, { &name, &frequency, &category, &magic, &element, &requirement, &mitigation });
            return csv.Read();
        }


        std::vector<Armor> Armor::Load(std::wistream& file)
        {
            Engine::Adapter::String<Armor> name(&Armor::name);
            Engine::Adapter::Integer<Armor> frequency(&Armor::frequency);
            Engine::Adapter::Struct<Armor, Requirement> requirement(&Armor::requirement, { &reqstr, &reqwis });
            Engine::Adapter::Enumeration<Armor, Covers> covers(&Armor::cover, coverMap);
            Engine::Adapter::Enumeration<Armor, Material::Category> material(&Armor::material, materialCategoryMap);
            Engine::Adapter::Struct<Armor, Damage> mitigation(&Armor::mitigation, damageAdapters);

            Engine::CSV<Armor> csv(file, {&name, &frequency, &requirement, &covers, &material, &mitigation});
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

    }

    Requirement Requirement::operator+(const Requirement& other)
    {
        return Requirement(strength + other.strength, wisdom + other.wisdom);
    }

    Requirement Requirement::operator*(int multiplier)
    {
        return Requirement(strength + multiplier, wisdom * multiplier);
    }
    Damage Damage::operator+(const Damage& other)
    {
        return Damage(sharp + other.sharp, crush + other.crush, fire + other.fire, disease + other.disease, spirit+other.spirit);
    }

    Armor::Armor(Type::Armor& type, Type::Armor::Material& material, Type::Armor::Bonus& bonus) :
        type(type),
        material(material),
        bonus(bonus)
    {
    }

    std::wstring Armor::Name() const
    {
        return material.name + L" " + type.name;
    }
    Requirement Armor::Required() const
    {
        return type.requirement + material.requirement * type.CoverCount();
    }

    Damage Armor::Mitigation() const
    {
        return type.mitigation + material.mitigation;
    }
}