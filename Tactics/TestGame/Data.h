#pragma once
#include <sstream>
#include "Game/Body.h"
#include "Game/Item.h"
#include "Game/Skills.h"

namespace Game
{
namespace Test
{

struct Data
{
    struct Simple : Body
    {
        Simple();
        std::wstringstream stream;
    };

    struct Human : Body
    {
        Human();
        std::wstringstream stream;
    };
    
    struct Blade : Weapon
    {
        Blade();
        Type::Weapon weaponType;
        Type::Weapon::Material mat;
        Type::Weapon::Modifier mod;
    };

    struct Combo
    {
        Combo();
        Skill first;
        Skill second;
    };
};

}
}