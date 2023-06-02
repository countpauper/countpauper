import pytest

from character import Character
from generate_character import random_character, random_monster
from items import Weapon, RangedWeapon, Shield, Armor, Equipment
from dice import Dice

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
    assert Character(physical=3, inventory=[Weapon(enchantment=1)]).attack_dice() == Dice(6)+1
    assert Character(physical=3, inventory=[Weapon(heavy=True, enchantment=2)]).attack_dice() == Dice(6,4)+2
    assert Character(physical=3, inventory=[Weapon(), Weapon()]).attack_dice(1) == Dice(6)




def test_auto_equip():
    assert Character(inventory=[Equipment(name='Test')]).main_hand() is None
    assert type(Character(inventory=[Weapon()]).main_hand()) == Weapon
    assert Character(inventory=[Weapon(name='Sword'), Weapon(name='Dagger')]).off_hand().name == 'Dagger'
    assert Character(inventory=[Weapon(heavy=True), Weapon()]).off_hand() is None
    assert Character(inventory=[RangedWeapon(), Weapon()]).off_hand() is None
    assert type(Character(inventory=[Shield()]).off_hand()) == Shield
    assert type(Character(inventory=[Armor()]).worn[0]) == Armor


def test_str():
    assert str(Character(name='Foo', level=2, physical=5, mental=3, hp=2, inventory=[Weapon(name="Testsword"),Shield()]))=="""Foo: Level 2
    5 Physical: 2/6 HP
    3 Mental: 3/3 SP
    1 Social: 1/1 MP
    Defense 1d10 Attack 1d10
    Inventory[2/5] Testsword shield 
    """


def test_random_character():
    c = random_character(1)
    assert(c.physical + c.mental + c.social == 7)
    assert(c.level == 1)
    assert(c.name)
    assert(c.carried() <= c.capacity())



def test_random_leveled_character():
    c = random_character(3)
    assert(c.physical + c.mental + c.social == 9)
    assert(c.level == 3)
    assert(c.name)
    assert(c.carried() <= c.capacity())


def test_random_monster():
    c = random_monster(5)
    assert(c.physical + c.mental + c.social == 11)
    assert(c.level == 5)
    assert(c.name)


def test_random_weak_monster():
    c = random_monster(-3)
    assert(c.physical+ c.mental + c.social == 3)
    assert(c.physical>=0)
    assert(c.mental>=0)
    assert(c.social>=0)
    assert(c.level == -3)
    assert(c.name)

    with pytest.raises(ValueError):
        c=random_monster(-4)