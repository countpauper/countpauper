from generate_character import random_character, random_monster
from skills import Skill
import pytest


def test_random_character():
    c = random_character(1)
    assert(c.physical + c.mental + c.social == 7)
    assert(c.level == 1)
    assert(c.name)
    assert(c.carried() <= c.capacity())


def test_random_leveled_character():
    c = random_character(3)
    assert(c.physical + c.mental + c.social == 9)
    assert(c.level == 3)
    assert(c.name)
    assert(c.carried() <= c.capacity())
    assert(len(c.skill) == c.level)
    assert(isinstance(c.skill[0], Skill))

def test_random_monster():
    c = random_monster(5)
    assert(c.physical + c.mental + c.social == 11)
    assert(c.level == 5)
    assert(c.name)


def test_random_weak_monster():
    c = random_monster(-3)
    assert(c.physical+ c.mental + c.social == 3)
    assert(c.physical>=0)
    assert(c.mental>=0)
    assert(c.social>=0)
    assert(c.level == -3)
    assert(c.name)

    with pytest.raises(ValueError):
        c=random_monster(-4)

