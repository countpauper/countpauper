#include "Data.h"
#include "Game/Damage.h"

namespace Game
{
namespace Test
{
namespace Data
{

    Simple::Simple() :
        stream(
        L"1\n"
        L"Body All[0-2] Full All 6 ,,,,\n"
        )
    {
        Add(Body::Part(L"All", Anatomy(Plane::All, 0, 3), Slot::Full, Stats(6, 6, 6, 6, 6), Damage()));
    }

    Human::Human() :
        stream(
        L"6 \n"
        L"Head All[4] Head Int 10 ,,,, \n"
        L"Chest Sagittal[3] Chest Wis 10 ,,,,\n"
        L"Belly Sagittal[2] Belly Con 10 ,,,,\n"
        L"RArm Right[2-3] Arms Str 6 ,,,,\n"
        L"LArm Left[2-3] Arms Str 4 ,,,,\n"
        L"Legs Around[0-1] Legs Agi 10 ,,,,\n"
        )
    {
        Add(Body::Part(L"Head", Anatomy(Plane::All, 4, 1), Slot::Head, Stats(Attribute::Intelligence, 10), Damage()));
        Add(Body::Part(L"Chest", Anatomy(Plane::Sagittal, 3, 1), Slot::Chest, Stats(Attribute::Wisdom, 10), Damage()));
        Add(Body::Part(L"Belly", Anatomy(Plane::Sagittal, 2, 1), Slot::Belly, Stats(Attribute::Constitution, 10), Damage()));
        Add(Body::Part(L"RArm", Anatomy(Plane::Right, 2, 2), Slot::Arms, Stats(Attribute::Strength, 6), Damage()));
        Add(Body::Part(L"LArm", Anatomy(Plane::Left, 2, 2), Slot::Arms, Stats(Attribute::Strength, 4), Damage()));
        Add(Body::Part(L"Legs", Anatomy(Plane::Around, 0, 2), Slot::Legs, Stats(Attribute::Agility, 10), Damage()));
    }

    Blade::Blade() :
        Weapon(weaponType, mat, mod),
        weaponType(Type::Weapon::Style::Blade, 1, ::Game::Damage(Wound::Type::Sharp, Score(L"TestBlade", 1)))
    {
    }

    Shield::Shield() :
        Weapon(weaponType, mat, mod),
        weaponType(Type::Weapon::Style::Shield, 1, ::Game::Damage())
    {
    }

    Harnass::Harnass() :
        Armor(type, material, mod),
        type(Slot::Body, Type::Armor::Category::Metal, ::Game::Damage(Wound::Type::Sharp, Score(L"TestHarnass", 4)))
    {

    }


    Melee::Melee() : Skill(L"Melee", Skill::Trigger::Act, Type::Weapon::Style::Blade, Trajectory::Straight, 1)
    {
        categories.insert(L"1");
    }

    Combo::Combo() : Skill(L"Combo", Skill::Trigger::Combo, Type::Weapon::Style::Blade, Trajectory::Straight, 1)
    {
        follows.insert(L"1");
    }

	Buff::Buff() : Skill(L"Buff", Attribute::Intelligence, Targeting::Self)
	{
	}

    Attacker::Attacker() :
        State(BaseState::_body, Position(0, 0), Direction(), 10, {}, { &weapon }, { &skill, &combo, &buff })
    {
        auto& arm = body[L"RArm"];
        arm.Hold(weapon);
    }

    Victim::Victim() :
        State(BaseState::_body, Position(1, 0), Direction(), 10, {}, {}, {})
    {
    }

}
}
}