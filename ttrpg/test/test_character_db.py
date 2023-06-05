import pytest
from character import Character
from items import *
from skills import Parry
from effect import Effect
from errors import CharacterUnknownError

def test_create_db(db):
    assert db

def test_character_stats(db):
    c = Character(name="foo", level=2, mental=3, social=4, physical=5)
    c.hp, c.pp, c.mp, c.ap = 4, 1, 2, 0
    db.store("guild", "user", c)
    restored = db.retrieve("guild", "user", "foo")
    assert id(c) != id(restored)
    assert restored.name == 'foo'
    assert restored.level == c.level
    assert restored.mental == c.mental
    assert restored.social == c.social
    assert restored.physical == c.physical
    assert restored.hp == c.hp
    assert restored.mp == c.mp
    assert restored.pp == c.pp
    assert restored.ap == c.ap
    assert restored.portrait is None
    assert restored.color is None


def test_update(db):
    c = Character(name="foo")
    db.store("guild","user", c)
    c.physical = 99
    db.store("guild", "user", c)
    res = db.connection.execute("SELECT Count(Id) FROM character;")
    assert res.fetchone()[0] == 1
    c = db.retrieve("guild", "user", c.name)
    assert c.physical == 99


def test_retrieve_default_character(db):
    c = Character(name="foo", level=2, mental=3, social=4, physical=5)
    db.store("guild", "user", c)
    restored = db.retrieve("guild", "user")
    assert restored.name == 'foo'


def test_cannot_retrieve_default_opponent(db):
    db.store("guild", "user", Character())
    with pytest.raises(CharacterUnknownError):
        db.retrieve("guild", None)


def test_user(db):
    c = Character(name="foo", level=2, mental=3, social=4, physical=5)
    db.store("guild", "owner", c)
    assert db.user("guild", c.name)

def test_retrieve_opponent(db):
    c = Character(name="foo", level=2, mental=3, social=4, physical=5)
    db.store("guild", "opponent", c)
    restored = db.retrieve("guild", None, "foo")
    assert restored is not None


def test_character_flavour(db):
    c=Character(name="Name with Space", portrait="http://www.unittest.com/portrait.jpg", color="#blue")
    db.store("guild","user", c)
    restored = db.retrieve("guild", "user", "Name with Space")
    assert c.name == restored.name
    assert c.portrait == restored.portrait
    assert c.color == restored.color


def test_inventory(db):
    c = Character(name="Rich", physical=6, inventory=[
                  MeleeWeapon(name="Golden Sword", enchantment=1),
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

def test_skills(db):
    c = Character(skill=[Parry()])
    db.store("guild","user", c)
    c = db.retrieve("guild", "user", c.name)
    assert c.skill
    assert isinstance(c.skill[0], Parry)


def test_effects(db):
    c=Character(name="Stronk", physical=5)
    c.effects.append(Effect("stronger",1, dict(physical=dict(bonus=2))))
    db.store("guild", "owner", c)
    c=db.retrieve("guild", "owner", "Stronk")
    assert c.effects
    assert c.physical == 7


def test_exists(db):
    c = Character()
    assert not db.exists("guild","user",c.name)
    db.store("guild", "user", c)
    assert db.exists("guild","user",c.name)

def test_exists_opponent(db):
    c = Character()
    assert not db.exists("guild", None, c.name)
    db.store("guild", "user", c)
    assert db.exists("guild", None, c.name)


def test_delete(db):
    c = Character()
    with pytest.raises(CharacterUnknownError):
        db.delete("guild", "user", c.name)
    db.store("guild", "user", c)
    assert db.delete("guild", "user", c.name)
