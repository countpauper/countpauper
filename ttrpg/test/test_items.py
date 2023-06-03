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


def test_melee_weapon():
    assert str(MeleeWeapon())
    assert str(RangedWeapon(name="chakram")) == 'chakram'
    assert MeleeWeapon().hands() == 1
    assert MeleeWeapon().weight() == 1
    assert MeleeWeapon().bonus() == Dice()
    assert MeleeWeapon().reach() == 0
    assert MeleeWeapon(enchantment=2).bonus() == Dice()+2


def test_heavy_melee_weapon():
    assert str(MeleeWeapon(heavy=True))
    assert MeleeWeapon(heavy=True).hands() == 2
    assert MeleeWeapon(heavy=True).weight() == 2
    assert MeleeWeapon(heavy=True).bonus() == Dice(4)
    assert MeleeWeapon(heavy=True).reach() == 0
    assert MeleeWeapon(heavy=True, enchantment=1).bonus() == Dice(4)+1


def test_ranged_weapon():
    assert str(RangedWeapon())
    assert str(RangedWeapon(name='boomerang')) == 'boomerang'
    assert RangedWeapon().hands() == 2
    assert RangedWeapon().weight() == 1
    assert RangedWeapon().bonus() == Dice()
    assert RangedWeapon().reach() == 2
    assert RangedWeapon(heavy=True).reach() == 3


def test_factory():
    assert isinstance(ItemFactory("shield"), Shield)
    assert isinstance(ItemFactory("sword"), Weapon)
    assert ItemFactory("katana").heavy
    assert isinstance(ItemFactory("crossbow"), RangedWeapon)
    assert not ItemFactory("sling").heavy
    assert isinstance(ItemFactory("gambeson"), Armor)
    assert ItemFactory("brigandine").rating==3
    assert isinstance(ItemFactory("crowbar"), Equipment)
