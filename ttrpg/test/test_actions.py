from character import Character
from items import Shield
from errors import GameError
from dice import Dice
import pytest

def test_turn():
    c = Character()
    c.ap=0
    c.turn()
    assert c.ap == c.max_ap()


def test_attack():
    attacker = Character(physical=2)
    result = attacker.attack()
    assert attacker.ap == 2
    assert '1d4' in result


def test_attack_target():
    attacker = Character()
    target = Character()
    result = attacker.attack(target)
    assert attacker.name in result
    assert target.name in result
    assert "hits" in result or "misses" in result
    assert attacker.ap == 2


def test_attack_no_ap():
    # TODO: attacker and target as fixtures?
    attacker = Character()
    attacker.ap=0
    with pytest.raises(GameError):
        attacker.attack()



def test_cover():
    c = Character(physical=2, inventory=[Shield()])
    c.auto_equip()
    item = c.cover()
    assert c.ap == 2
    assert c.defense_dice() == Dice(4,1)
    assert isinstance(item, Shield)
    with pytest.raises(GameError):
        c.cover()
    assert c.ap == 2
