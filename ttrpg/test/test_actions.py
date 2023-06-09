from character import Character
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
    result = attacker.attack()
    assert attacker.ap == 2
    assert re.match(r"1d4 \(\d\) = \d", str(result))
    assert result.damage() is None


def test_attack_target():
    attacker = Character(physical=2)
    target = Character(physical=2)
    result = attacker.attack(target)
    str_result=str(result)
    if result.hit():
        assert re.match(r"1d\d \(\d\) = \d VS 1d\d \(\d\) = \d hits for \d damage", str(result))
    else:
        assert re.match(r"1d\d \(\d\) = \d VS 1d\d \(\d\) = \d misses", str(result))
    assert attacker.ap == 2


def test_attack_no_ap():
    # TODO: attacker and target as fixtures?
    attacker = Character()
    attacker.ap=0
    with pytest.raises(GameError):
        attacker.attack()


def test_cover():
    c = Character(physical=3, inventory=[Shield()])
    c.auto_equip()
    item = c.cover()
    assert c.ap == 2
    assert c.defense_dice() == Dice(4,1)
    assert isinstance(item, Shield)
    with pytest.raises(GameError):
        c.cover()
    assert c.ap == 2

def test_too_weak_to_skill():
    c = Character(physical=0, skill=[Parry], inventory=[MeleeWeapon(name="sword")])
    with pytest.raises(GameError):
        c.execute(Parry)

def test_execute_skill():
    c = Character(physical=2, skill=[Parry], inventory=[MeleeWeapon(name="sword")])
    result = c.execute(Parry)
    assert c.affected('parry')
    assert c.ap == 2
    assert result == f"""parries with a sword"""


def test_unknown_skill():
    c=Character(skill=[Parry])
    with pytest.raises(GameError):
        result = c.execute("knit")


def test_skill_ability_0():
    c = Character(physical=0, skill=[Parry])
    with pytest.raises(GameError):
        c.execute(Parry)
