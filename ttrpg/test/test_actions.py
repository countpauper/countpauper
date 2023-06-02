from character import Character
from errors import GameError
import pytest


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


def test_turn():
    c = Character()
    c.ap=0
    c.turn()
    assert c.ap == c.max_ap()
