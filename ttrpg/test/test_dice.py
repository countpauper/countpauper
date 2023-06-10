from dice import Dice
import d20
import re

def test_roll_one():
    d = Dice(6)
    r = d.roll()
    assert len(r.expr.children)==1
    assert r.expr.children[0].num==1
    assert r.expr.children[0].size==6


def test_roll_two_different():
    r = Dice(4, 6).roll()
    assert r.expr.children[0].op=='+'
    terms=r.expr.children[0].children
    assert len(terms)==2
    assert terms[0].num==1
    assert terms[0].size==4
    assert terms[1].num==1
    assert terms[1].size==6


def test_roll_two_same():
    r = Dice(6, 6).roll()
    assert len(r.expr.children) == 1
    assert r.expr.children[0].num == 2
    assert r.expr.children[0].size == 6


def test_roll_with_bonus():
    r = (Dice(6)+2).roll()
    assert r.expr.children[0].op == '+'
    terms=r.expr.children[0].children
    assert len(terms) == 2
    assert len(terms) == 2
    assert terms[0].num == 1
    assert type(terms[1]) == d20.Literal
    assert terms[1].number == 2

def test_none():
    d = Dice()
    r = d.roll()
    assert r.total == 0


def test_str():
    assert str(Dice()) == '0'
    assert str(Dice(6)) == '1d6'
    assert str(Dice(6,6)) == '2d6'
    assert str(Dice(4,6)) == '1d4+1d6'
    assert str(Dice(10)+2) == '1d10+2'
    assert str(Dice(8)-1) == '1d8-1'
    assert str(Dice(8, 1)) == '1d8+1'
    assert str(Dice(10,0)) == '1d10+0'


def test_result_str():
    assert str(Dice().roll()) == '0'
    assert str(Dice(1).roll()) == '1'
    assert re.match(r"1d4 \(\d\) = \d", str(Dice(4).roll()))


def test_add():
    assert Dice(6)+Dice(4) == Dice(6,4)
    assert Dice(10)+Dice(1) == Dice(10)+1
    assert Dice()+Dice(8) == Dice(8)
    assert (Dice(2)+1)+(Dice(3)-1) == Dice(2,3)+1-1
    d = Dice(4)
    d += Dice(2)
    d += 1
    assert d == Dice(4, 2)+1


def test_add_boni():
    assert Dice(4) + dict() == Dice(4)
    assert Dice(6) + dict(bonus=1) == Dice(6, 1)
    assert Dice(8) + dict(a=2, b=3) == Dice(8, bonus=[2,3])
    assert Dice(10) + dict(d=Dice(4, 1), e=2) == Dice(10, 4, bonus=[1,2])


def test_sub():
    assert Dice(6)-Dice(4) == Dice(6,-4)
    assert Dice(10)-1 == Dice(10, -1)

def test_fix():
    assert Dice.fix(1) is None
    assert Dice(20).roll().total == 1
    assert Dice.fix(None) == 1
    assert Dice(20, 1).roll().total > 1


def test_minimum():
    assert Dice(6).minimum() == 1
    assert Dice(4, -1).minimum() == 0
    assert Dice(4, 6, 0).minimum() == 2
    assert Dice(6, 6, 6).minimum() == 3


def test_maximum():
    assert Dice(6).maximum() == 6
    assert Dice(4, -1).maximum() == 3
    assert Dice(4, 6, 0).maximum() == 10
    assert Dice(6, 6, 6).maximum() == 18

def test_fix_min(dice_min):
    assert Dice(6).roll().total == 1
    assert Dice(4, 4, 6).roll().total == 3

def test_fix_max(dice_max):
    assert Dice(6).roll().total == 6
    assert Dice(4, 4, 6).roll().total == 14

def test_for_ability():
    assert Dice.for_ability(0) == Dice()
    assert Dice.for_ability(2) == Dice(2)
    assert Dice.for_ability(21) == Dice(12,12,12)
