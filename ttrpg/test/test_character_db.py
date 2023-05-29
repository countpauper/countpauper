from character_db import CharacterDB
from character import Character
from items import *

import pytest
import os

def test_create_db(db):
    assert db

def test_character_stats(db):
    c = Character(name="foo", level=2, mental=3, social=4, physical=5)
    db.store("guild","user", c)
    restored = db.retrieve("guild", "user", "foo")
    assert id(c) != id(restored)
    assert restored.name == 'foo'
    assert restored.level == c.level
    assert restored.mental == c.mental
    assert restored.social == c.social
    assert restored.physical == c.physical
    assert restored.hp == c.hp
    assert restored.mp == c.mp
    assert restored.sp == c.sp
    assert restored.portrait is None
    assert restored.color is None


def test_character_flavour(db):
    c=Character(name="Name with Space", portrait="http://www.unittest.com/portrait.jpg", color="#blue")
    db.store("guild","user", c)
    restored = db.retrieve("guild", "user", "Name with Space")
    assert c.name == restored.name
    assert c.portrait == restored.portrait
    assert c.color == restored.color


def test_inventory(db):
    c = Character(name="Rich", physical=6, inventory=[
                  Weapon(name="Golden Sword", enchantment=1),
                  RangedWeapon(heavy=True, name="Birch bow"),
                  Armor(name="Mithril Chainmail", rating=2),
                  Shield(),
                  Equipment("Gold")])
    db.store("guild","user", c)
    c = db.retrieve("guild", "user", "Rich")
    assert c.carried() == 6
    assert c.defense_dice() == Dice(6,4)+2
    assert c.off_hand().name == "shield"
    assert c.main_hand().name == "Golden Sword"
    assert len(c.item("Birch bow")) == 1


def test_exists(db):
    c=Character()
    assert not db.exists("guild","user",c.name)
    db.store("guild", "user", c)
    assert db.exists("guild","user",c.name)

def test_delete(db):
    c = Character()
    with pytest.raises(RuntimeError):
        db.delete("guild", "user", c.name)
    db.store("guild", "user", c)
    assert db.delete("guild", "user", c.name)
