from character import Character
from items import MeleeWeapon, RangedWeapon, Shield, Armor, Equipment
from errors import GameError
from dice import Dice
import pytest

def test_default():
    c = Character()
    assert c.level == 1
    assert c.physical == 1
    assert c.mental == 1
    assert c.social == 1
    assert not c.inventory
    assert not c.skill


def test_specific_abilities():
    assert Character(physical=4).physical == 4
    assert Character(mental=5).mental == 5
    assert Character(social=6).social == 6


def test_max_points():
    assert Character(level=1, physical=3).max_hp() == 5
    assert Character(mental=5).max_sp() == 5
    assert Character(social=6).max_mp() == 6


def test_specific_points():
    assert Character(hp=1).hp == 1
    assert Character(sp=0).sp == 0
    assert Character(social=4, mp=2).mp == 2


def test_default_flavour():
    c = Character()
    assert c.name == 'Nemo'
    assert c.portrait is None
    assert c.color is None


def test_status():
    c = Character()
    assert c.alive()
    assert c.moralized()
    assert c.active()
    c.damage(c.hp)
    assert not c.alive()
    assert not c.active()


def test_defense():
    assert Character(physical=2).defense_dice() == Dice(4)
    assert Character(physical=3, inventory=[Armor(rating=2)]).defense_dice() == Dice(6)+2


def test_attack_dice():
    assert Character(physical=6).attack_dice() == Dice(6,4)
    assert Character(physical=2).attack_dice(1) == Dice(4)-2
    assert Character(physical=2).attack_dice(2) == Dice(4)-4
    assert Character(physical=2).attack_dice(3) == Dice(4)-6
    assert Character(physical=2).attack_dice(1, 1) == Dice(4)-2+1
    assert Character(physical=3, inventory=[MeleeWeapon(enchantment=1)]).attack_dice() == Dice(6)+1
    assert Character(physical=3, inventory=[MeleeWeapon(heavy=True, enchantment=2)]).attack_dice() == Dice(6,4)+2
    assert Character(physical=3, inventory=[MeleeWeapon(), MeleeWeapon()]).attack_dice(1) == Dice(6)


def test_auto_equip():
    assert Character(inventory=[Equipment(name='Test')]).main_hand() is None
    assert type(Character(inventory=[MeleeWeapon()]).main_hand()) == MeleeWeapon
    assert Character(inventory=[MeleeWeapon(name='Sword'), MeleeWeapon(name='Dagger')]).off_hand().name == 'Dagger'
    assert Character(inventory=[MeleeWeapon(heavy=True), MeleeWeapon()]).off_hand() is None
    assert Character(inventory=[RangedWeapon(), MeleeWeapon()]).off_hand() is None
    assert type(Character(inventory=[Shield()]).off_hand()) == Shield
    assert type(Character(inventory=[Armor()]).worn[0]) == Armor


def test_str():
    assert str(Character(name='Foo', level=2, physical=5, mental=3, hp=2, inventory=[MeleeWeapon(name="Testsword"),Shield()]))=="""Foo: Level 2
    5 Physical: 2/6 HP
    3 Mental: 3/3 SP
    1 Social: 1/1 MP
    Defense 1d10 Attack 1d10
    Inventory[2/5] Testsword shield 
    """

def test_obtain():
    c = Character(physical=2)
    c.obtain("sword", Shield)
    assert c.carried() == 2


def test_obtain_over_capacity():
    c = Character(physical=2)
    with pytest.raises(GameError):
        c.obtain("spear", Armor())


def test_lose():
    c = Character(inventory=[MeleeWeapon(name="practice"), Armor(rating=1)])
    c.auto_equip()
    c.lose("practice", Armor)
    assert c.carried() == 0
    assert c.main_hand() == None
    assert c.worn == []

def test_cant_lose():
    c = Character(inventory=[MeleeWeapon(name="practice")])
    c.auto_equip()
    with pytest.raises(GameError):
        c.lose("practice", "cheese")
    assert c.carried() == 1
    assert c.main_hand().name == "practice"
