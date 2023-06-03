from character import Character
from skills import *
import re
import pytest

def test_parry():
    c=Character(physical=2, inventory=[])
    skill = Parry()
    assert skill.cost == dict(ap=1)
    with pytest.raises(GameError):  # need a weapon
        skill(c)
    c.obtain(MeleeWeapon(name='spear', heavy=True))
    c.auto_equip()
    result = skill(c)
    assert c.affected('parry')
    assert c.defense_dice() == Dice(4, 1)
    assert result == "parries with a spear"

    with pytest.raises(GameError):  # already done
        skill(c)
    c.turn()
    assert not c.affected('parry')  # duration 1


def test_crosscut():
    c = Character(physical=2, mental=1, inventory=[MeleeWeapon(name='dagger')])
    t = Character(physical=0, name='target')
    skill = CrossCut()
    assert skill.cost == dict(ap=2, sp=1)
    with pytest.raises(GameError):  # need to dual wield
        skill(c, t)
    c.obtain(MeleeWeapon(name='axe'))
    c.auto_equip()
    result = skill(c, t)
    assert re.match(r"crosscuts Target\: 2d4 \(\d, \d\) = \d VS 1 hits for \d damage.", result)


def test_repeat_parry():
    c=Character(physical=2, inventory=[MeleeWeapon(name='spear', heavy=True)])
    skill = Parry()
