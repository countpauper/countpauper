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

def test_lambda_range():
    current_bonus = 0
    c = Counter(maximum=3, temporary=lambda: current_bonus)
    assert c == 3
    current_bonus = 2
    assert c == 5

def test_change_max():
    current_bonus = 0
    current_max = 5
    c = Counter(maximum=lambda: current_max, temporary=lambda: current_bonus)
    c += 3
    assert c == 5
    current_max = 10
    assert c == 5
    assert str(c) == "5/10"
    c += 7
    assert c == 10
    assert str(c) == "10/10"
    current_max = 5
    assert c == 5
    assert str(c) == "5/5"

def test_counter_temporary():
    current_bonus = 0
    a = Counter(maximum=3, temporary=lambda: current_bonus)
    a += 2
    assert a == 3

    current_bonus = 2
    assert a == 5
    assert str(a) == "5/5"

    a -= 4
    assert a == 1

    current_bonus = -1
    assert a == 0

    a += 3
    assert str(a) == "2/2"


def test_change_and_set():
    current_max = 3
    current_bonus = 0
    a = Counter(maximum=lambda: current_max,temporary=lambda: current_bonus)
    b = a + 2
    assert a == 3
    assert b == 3
    current_max = 5
    assert a == 3
    assert b == 3
    c = b - 6
    d = b + 3
    assert c == 0
    assert d == 5
    current_bonus = 1
    assert str(a) == "4/6"
    assert str(b) == "4/6"
    assert str(c) == "1/6"
    assert str(d) == "6/6"

