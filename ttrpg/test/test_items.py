from items import *

def test_equipment():
    assert Equipment(name="rope").weight() == 0
    assert str(Equipment(name="rope")) == "rope"
    assert Equipment(name="tent", heavy=True).weight() == 1
    assert Equipment(name="cheese").hands() == 0

def test_shield():
    assert str(Shield()) == "shield"
    assert Shield().weight() == 1
    assert Shield().hands() == -1

def test_armor():
    assert str(Armor(name='scale mail')) == 'scale mail'
    assert Armor().hands() == 0
    assert Armor(rating=1).weight() == 1
    assert Armor(rating=2).weight() == 2
    assert Armor(rating=3).weight() == 3
    assert str(Armor(rating=1))
    assert str(Armor(rating=2))
    assert str(Armor(rating=3))
    assert Armor().get_boni('defense') == dict(rating=1)

def test_weapon():
    assert str(Weapon())
    assert str(Weapon(name="chakram")) == 'chakram'
    assert Weapon().hands() == 1
    assert Weapon().weight() == 1
    assert Weapon().bonus() == Dice()
    assert Weapon().reach() == 0
    assert Weapon(enchantment=2).bonus() == Dice()+2

def test_heavy_weapon():
    assert str(Weapon(heavy=True))
    assert Weapon(heavy=True).hands() == 2
    assert Weapon(heavy=True).weight() == 2
    assert Weapon(heavy=True).bonus() == Dice(4)
    assert Weapon(heavy=True).reach() == 0
    assert Weapon(heavy=True, enchantment=1).bonus() == Dice(4)+1

def test_ranged_weapon():
    assert str(RangedWeapon())
    assert str(RangedWeapon(name='boomerang')) == 'boomerang'
    assert RangedWeapon().hands() == 2
    assert RangedWeapon().weight() == 1
    assert RangedWeapon().bonus() == Dice()
    assert RangedWeapon().reach() == 2
    assert RangedWeapon(heavy=True).reach() == 3
