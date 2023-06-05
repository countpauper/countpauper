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

def test_repeat_parry():
    c=Character(physical=2, inventory=[MeleeWeapon(name='spear', heavy=True)])
    skill = Parry()

def test_crosscut():
    c = Character(physical=2, mental=1, inventory=[MeleeWeapon(name='dagger')])
    t = Character(physical=0, name='target')
    skill = CrossCut()
    assert skill.cost == dict(ap=1, pp=1)
    with pytest.raises(GameError):  # need to dual wield
        skill(c, t)
    c.obtain(MeleeWeapon(name='axe'))
    c.auto_equip()
    result = skill(c, t)
    assert re.match(r"crosscuts Target\: 2d4 \(\d, \d\) = \d VS 1 hits for \d damage.", result)


def test_explosion_resist(dice_min):
    c = Character(mental=3, inventory=[MeleeWeapon(name='dagger')])
    t = [Character(level=2, physical=0, name=f'target_{idx}') for idx in range(2)]
    skill = Explosion()
    assert skill.cost == dict(ap=1, pp=3)
    result = skill(c, *t)
    assert result == f"explodes (1)\n  Target_0 evades (1)\n  Target_1 evades (1)"
    assert t[0].hp == t[0].max_hp()

def test_explosion_hit(dice_max):
    c = Character(mental=3, inventory=[MeleeWeapon(name='dagger')])
    t = [Character(level=1, physical=idx, name=f'target_{idx}') for idx in range(2)]
    skill = Explosion()
    assert skill.cost == dict(ap=1, pp=3)
    result = skill(c, *t)
    assert result == "explodes (6)\n  Target_0 [0/5] hit (1) for 5 damage\n  Target_1 [1/5] hit (2) for 4 damage"
    assert t[0].hp == 0
    assert t[1].hp == 1
