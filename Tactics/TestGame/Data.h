#pragma once
#include <sstream>

#include "Figment.h"
#include "Game/Body.h"
#include "Game/Item.h"
#include "Game/Skill.h"

namespace Game
{
namespace Test
{

namespace Data
{
struct Simple : Anatomy
{
    Simple();
    std::wstringstream stream;
};

struct Human : Anatomy
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

struct Wand : Weapon
{
	Wand();
	Type::Weapon weaponType;
	Type::Weapon::Material mat;
	Type::Weapon::Modifier mod;
};

struct Shield : Weapon
{
    Shield();
    Type::Weapon weaponType;
    Type::Weapon::Material mat;
    Type::Weapon::Modifier mod;
};

struct Harnass : Armor
{
    Harnass();
    Type::Armor type;
    Type::Armor::Material material;
    Type::Armor::Modifier mod;
};

struct PlotArmor : Armor
{
	PlotArmor();
	Type::Armor type;
	Type::Armor::Material material;
	Type::Armor::Modifier mod;
};

struct Melee : Skill
{
    Melee();
};

struct Combo : Skill
{
    Combo();
};

struct Zap : Skill
{
	Zap();
};

struct Buff : Skill
{
	Buff();
};
// Base state so body is initialized before actual State
template<class AnatomyType>
class BaseState
{
public:
	BaseState()
	{
	}
protected:
	AnatomyType anatomy;
};

struct Knight : private BaseState<Human>, public Figment
{
	Knight();
    Data::Blade weapon;
    Data::Melee melee;
	Data::Combo combo;
};

struct Mage : private BaseState<Human>, public Figment
{
	Mage();
	Data::Wand weapon;
	Data::Zap zap;
	Data::Buff buff;
};
struct Victim : private BaseState<Simple>, public Figment
{
	Victim();
};

struct MiniMap : public FlatMap
{
    MiniMap();
    std::wstringstream stream;
};

}
}
}