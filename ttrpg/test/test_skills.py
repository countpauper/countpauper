from character import Character
from skills import *
import pytest
from errors import *


def test_description():
    assert all(skill.help() for skill in Skill.all)

def test_parry():
    c=Character(physical=3, skill=[Parry], inventory=[])
    skill = Parry(c)
    assert skill.cost == dict(ap=1)
    assert skill.ability == Physical
    assert not skill.offensive
    with pytest.raises(GameError):  # need a weapon
        skill()
    c.obtain(MeleeWeapon(name='spear', heavy=True))
    c.auto_equip()
    result = skill()
    assert c.affected('parry')
    assert c.defense_dice() == Dice(4, 1)
    assert result == "parries with a spear"

    with pytest.raises(GameError):  # already done
        skill(c)
    c.turn()
    assert not c.affected('parry')  # duration 1

def test_repeat_parry():
    c=Character(physical=2, skill=[Parry], inventory=[MeleeWeapon(name='spear', heavy=True)])
    skill = Parry(c)
    skill()
    with pytest.raises(GameError):
        skill()


def test_crosscut():
    skill = CrossCut(None)
    assert skill.cost == dict(ap=1, pp=1)
    assert skill.ability == Physical
    assert skill.offensive

def test_crosscut_hit(dice_max):
    c = Character(physical=3, skill=[CrossCut], inventory=[MeleeWeapon(name='dagger')])
    t = Character(physical=1, name='target')
    skill = CrossCut(c)
    assert skill.cost == dict(ap=1, pp=1)
    with pytest.raises(GameError):  # need to dual wield
        skill(t)
    c.obtain(MeleeWeapon(name='axe'))
    c.auto_equip()
    result = skill(t)
    assert result == f"crosscuts Target: 8 VS 1 hits for 7 damage [0/5]"

def test_explosion():
    skill = Explosion(None)
    assert skill.cost == dict(ap=1, pp=3)
    assert skill.ability == Mental
    assert skill.offensive

def test_explosion_resist(dice_min):
    c = Character(mental=4, skill=[Explosion], inventory=[MeleeWeapon(name='dagger')])
    t = [Character(level=2, physical=1, name=f'target_{idx}') for idx in range(2)]
    skill = Explosion(c)
    result = skill(*t)
    assert result == f"explodes (1)\n  Target_0 evades (1)\n  Target_1 evades (1)"
    assert t[0].hp == t[0].max_hp()

def test_explosion_hit(dice_max):
    c = Character(mental=4, skill=[Explosion], inventory=[MeleeWeapon(name='dagger')])
    t = [Character(level=1, physical=1 + idx, name=f'target_{idx}') for idx in range(2)]
    skill = Explosion(c)
    assert skill.cost == dict(ap=1, pp=3)
    result = skill(*t)
    assert result == "explodes (6)\n  Target_0 [0/5] hit (1) for 5 damage\n  Target_1 [1/5] hit (2) for 4 damage"
    assert t[0].hp == 0
    assert t[1].hp == 1

def test_frighten(dice_max):
    c = Character(social=4, skill=[Frighten])
    t = Character(mental=2, social=3, name='target')
    skill = Frighten(c)
    assert skill.cost == dict(ap=1, pp=1)
    assert skill.ability == Social
    assert skill.offensive
    result = skill(t)
    assert result == "frightens Target: 6 VS 2 for 4 morale [0/3]"

def test_heal(dice_max):
    c = Character(mental=3, skill=[Heal])
    t = Character(name='target', hp=2)
    skill = Heal(c)
    assert skill.cost == dict(ap=1, pp=1)
    assert skill.ability == Mental
    assert not skill.offensive
    result = skill(t)
    assert result == "heals Target: for 4 health [5/5]"
    assert t.hp == 5

def test_familiar():
    c = Character(social=2, skill=[Familiar])
    skill = Familiar(c)
    assert skill.cost == dict(ap=1, mp=1)
    assert skill.ability == Social
    assert not skill.offensive
    result = skill()
    assert result == "summons a familiar"
    assert (f:=c.ally("Nemo's familiar")) is not None
    assert f.hp == 1
    assert f.pp == 0
    assert f.mp == 3
    assert f.capacity() == 0
    assert f.attack_dice() == Dice()
    with pytest.raises(GameError):
        skill()

def test_encourage():
    c = Character(social=3, skill=[Encourage])
    skill = Encourage(c)
    assert skill.cost == dict(ap=1, mp=1)
    assert skill.ability == Social
    assert not skill.offensive

    t = Character(name='target', physical=2, mental=3, social=4)
    result = skill(t)
    assert result == "encourages Target"
    assert t.ability_dice(Physical) == Dice(2, 1)
    assert t.ability_dice(Mental) == Dice(4, 1)
    assert t.ability_dice(Social) == Dice(6, 1)
    with pytest.raises(TargetError):
        skill(c)
    with pytest.raises(TargetError):
        skill()

def test_harmony():
    c = Character(social=2, skill=[Harmony])
    skill = Harmony(c)
    assert skill.cost == dict(ap=1, mp=1)
    assert skill.ability == Social
    assert not skill.offensive

    t = [Character(name=f'target_{idx}', physical=1) for idx in range(3)]
    result= skill(*t[:2])
    assert result == "harmonizes Target_0 and Target_1"
    assert t[0].defense_dice() == Dice(1,1)
    assert t[1].defense_dice() == Dice(1,1)
    with pytest.raises(TargetError):
        skill(*t)
