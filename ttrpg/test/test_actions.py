from character import Character
from actions import Attack, Cover
from items import Shield, MeleeWeapon
from errors import GameError
from skills import Parry
from dice import Dice
import pytest
import re


def test_turn():
    c = Character()
    c.ap=0
    assert not c.ap
    c.turn()
    assert c.ap == c.max_ap()


def test_attack():
    attacker = Character(physical=3)
    attack = Attack(attacker)
    assert attack.cost['ap'] == 1
    assert attack.verb() == "attacks"
    result = attack()
    assert re.match(r"1d4 \(\d\) = \d", str(result))
    assert result.delta() == 0

def test_attack_target():
    attacker = Character(physical=2)
    target = Character(physical=2, name="Target")
    attack = Attack(attacker)
    result = attack(target)

    if result.hit():
        assert re.match(r"1d\d \(\d\) = \d VS Target 1d\d \(\d\) = \d\: \d health damage \[\d\/5\]", str(result))
    else:
        assert re.match(r"1d\d \(\d\) = \d VS Target 1d\d \(\d\) = \d\: miss", str(result))

def test_cover():
    c = Character(physical=3, inventory=[Shield()])
    c.auto_equip()
    cover = Cover(c)
    assert cover.cost['ap'] == 1
    assert cover.verb() == "seeks cover"
    result = cover()
    assert c.defense_dice() == Dice(4,1)
    assert str(result) == "Nemo is protected by their shield"
    with pytest.raises(GameError):
        cover()

def test_too_weak_to_skill():
    c = Character(physical=0, skill=[Parry], inventory=[MeleeWeapon(name="sword")])
    with pytest.raises(GameError):
        c.execute(Parry)

def test_execute_skill():
    c = Character(physical=2, skill=[Parry], inventory=[MeleeWeapon(name="sword")])
    result = c.execute(Parry)
    assert c.affected('parrying')
    assert c.ap == 2
    assert str(result) == f"""Nemo is parrying with a sword"""


def test_unknown_skill():
    c=Character(skill=[Parry])
    with pytest.raises(GameError):
        result = c.execute("knit")


def test_skill_ability_0():
    c = Character(physical=0, skill=[Parry])
    with pytest.raises(GameError):
        c.execute(Parry)
