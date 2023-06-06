from character import Character
from stats import Counter
from effect import Effect


def test_get_and_set_property():
    c = Character()
    assert c.portrait is None
    c.portrait = 'http://www.pictures_of.cheese.com/gouda.jpg'
    assert c.portrait == 'http://www.pictures_of.cheese.com/gouda.jpg'


def test_get_and_set_ability():
    c = Character(mental = 1)
    assert c.mental == 1
    c.mental = 2
    assert c.mental == 2


def test_buf_bonus():
    c = Character(physical=2)
    assert c.physical == 2
    c.affect(Effect('buf',None, dict(physical=dict(buf=1))))
    assert c.physical == 3


def test_penalty_limit():
    c = Character(physical=2)
    assert c.physical == 2
    c.affect(Effect('penalty',None, dict(physical=dict(test=-3))))
    assert c.physical == 0


def test_counter():
    c = Character(mental=2)
    assert c.pp == 2

def test_lambda_max():
    current_max = 5
    c = Counter(lambda: current_max)
    assert c == 5
    current_max = 4
    assert c == 4

def test_change_counter():
    current_max = 5
    c = Counter(lambda:current_max)
    c += 3
    assert c == 5
    current_max = 10
    assert c == 8
    assert str(c) == "8/10"

def test_change_and_set():
    current_max = 3
    a = Counter(lambda: current_max)
    b = a + 2
    assert a == 3
    assert b == 3
    current_max = 8
    assert a == 3
    assert b == 5
    c = b - 6
    assert c == 0
    current_max = 7
    assert str(a) == "3/7"
    assert str(b) == "5/7"
    assert str(c) == "0/7"
