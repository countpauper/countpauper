from character import Character
from effect import Effect


def test_ability_bonus():
    c = Character(social=1)
    c.affect(Effect("bonus", boni=dict(social=dict(bonus=1))))
    assert c.social == 2

def test_ability_penalty():
    c = Character(physical=3)
    c.affect(Effect("penalty", boni=dict(physical=dict(penalty=-1))))
    assert c.physical == 2

def test_ability_minimum():
    c=Character(mental=1)
    c.affect(Effect("penalty", boni=dict(mental=dict(penalty=-2))))
    assert c.mental == 0

def test_bonus_hp():
    c=Character(level=1)
    c.affect(Effect("bhp", boni=dict(hp=dict(bonus=5))))
    assert c.hp == 10
    assert c.max_hp() == 10
    c.hp -= 6
    assert c.alive()

def test_level_drain():
    c=Character(level=2)
    c.affect(Effect("drain", boni=dict(level=dict(drained=-1))))
    assert c.level == 1
    assert c.hp == 5
    c.affect(Effect("drain", boni=dict(level=dict(drained_more=-1))))
    assert c.level == 0
    assert not c.alive()

def test_brain_drain():
    c=Character(mental=2)
    c.affect(Effect("drain", boni=dict(mental=dict(drained=-2))))
    assert c.pp == 0

