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
            { L"Stone",Element::Stone },
            { L"Water",Element::Water},
            { L"Fire",Element::Fire },
            { L"Air", Element::Air },
            { L"Nature", Element::Nature },
        });


        std::vector<Armor::Material> Armor::Material::Load(std::wistream& file)
        {
            Engine::Adapter::String<Armor::Material> name(&Armor::Material::name);
            Engine::Adapter::Integer<Armor::Material> frequency(&Armor::Material::frequency);
            Engine::Adapter::Enumeration<Armor::Material, Type::Material::Category> category(&Armor::Material::category, materialCategoryMap);
            Engine::Adapter::Integer<Armor::Material> magic(&Armor::Material::magic);
            Engine::Adapter::Enumeration<Armor::Material, Element> element(&Armor::Material::element, elementMap);
            Engine::Adapter::Integer<Requirement> reqstr(&Requirement::strength);
            Engine::Adapter::Integer<Requirement> reqwis(&Requirement::wisdom);
            Engine::Adapter::Struct<Armor::Material,Requirement> requirement(&Armor::Material::requirement, { &reqstr, &reqwis });
            Engine::Adapter::Integer<Damage> sharp(&Damage::sharp);
            Engine::Adapter::Integer<Damage> crush(&Damage::crush);
            Engine::Adapter::Integer<Damage> fire(&Damage::fire);
            Engine::Adapter::Integer<Damage> disease(&Damage::disease);
            Engine::Adapter::Struct<Armor::Material, Damage> mitigation(&Armor::Material::mitigation, { &sharp, &crush, &fire, &disease });

            Engine::CSV<Armor::Material> csv(file, { &name, &frequency, &category, &magic, &element, &requirement, &mitigation });
            return csv.Read();

        }
    }
}