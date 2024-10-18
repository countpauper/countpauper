import pytest
from gvar import import_gvar
import math

fn=import_gvar("math/9695112b-3192-47eb-93c7-e579c6679bac.gvar", "fn")

def test_parse_numeric():
    assert fn.parse("3") == 3
    assert fn.parse("-1.0") == dict(neg=1.0)
    # TODO gets parsed as (5.4e) - (1) assert fn.parse("5.4e-1") == 0.54
    # To fix, may have to preparse the whole string and replace all e+ and e-  (and E+/-) with a marker
    # then right before float conversion reverse those markers
    # BUT beware of e being the constant e. It's only for e after a digit or period

def test_parse_constants():
    assert fn.parse("pi") == math.pi
    assert fn.parse("e+1") == dict(sum=[math.e,1])

def test_parse_operators():
    assert fn.parse("3*4") == dict(prod=[3,4])
    assert fn.parse("1+2+0") == dict(sum=[1,2,0])
    assert fn.parse("1+2*3") == dict(sum=[1,dict(prod=[2,3])])
    assert fn.parse("2^3") == dict(pow=[2,3])
    assert fn.parse("3-1-2") == dict(sub=[3,1,2])
    assert fn.parse("100/10") == dict(div=[100,10])
    with pytest.raises(RuntimeError):
        assert fn.parse("3+2+")

def test_parse_functions():
    assert fn.parse("log(3)") == dict(log=3)
    assert fn.parse("sum(1,2,3)") == dict(sum=dict(tuple=[1,2,3]))
    assert fn.parse("sin(3.14)+2") == dict(sum=[dict(sin=3.14), 2])
    assert fn.parse("1-cos(1.57)") == dict(sub=[1, dict(cos=1.57)])

def test_parse_brackets():
    assert fn.parse("(3)") == 3
    assert fn.parse("(3,1)") == dict(tuple=[3,1])
    assert fn.parse("(3+2)") == dict(sum=[3,2])
    assert fn.parse("(3+2)*4") == dict(prod=[dict(sum=[3,2]),4])
    assert fn.parse("1/(3+2)") == dict(div=[1,dict(sum=[3,2])])
    assert fn.parse("1^(sqrt(4)+3)") == dict(pow=[1,dict(sum=[dict(sqrt=4),3])])
    with pytest.raises(RuntimeError):
        assert fn.parse("(3+1")
    with pytest.raises(RuntimeError):
        assert fn.parse("1^(sqrt(4)+3") == dict(pow=[1,dict(sum=[dict(sqrt=4),3])])

def test_compute_operators():
    assert fn.compute(dict(sum=[1,2])) == 3
    assert fn.compute(dict(sum=[])) == 0
    assert fn.compute(dict(prod=[3,4])) == 12
    assert fn.compute(dict(prod=[3])) == 3
    assert fn.compute(dict(prod=[])) == 1
    assert fn.compute(dict(div=[4,2,2])) == 1
    with pytest.raises(ZeroDivisionError):
        assert fn.compute(dict(div=[3])) == 3
    assert fn.compute(dict(pow=[2,5])) == 32
                      
def test_compute_nested():
    assert fn.compute(dict(sum=[3,dict(div=[4,2])])) == 5.0

def test_compute_constants():
    assert fn.compute(2) == 2
    assert fn.compute(3.1) == 3.1
    assert fn.compute([3,2]) == [3,2]

def test_compute_functions():
    assert fn.compute(dict(factorial=4))==24

def test_present_constants():
    assert fn.present(3) == "3"
    assert fn.present(-1.5) == "-1.5"
    assert fn.present([1,2]) == "[1, 2]"

def test_present_functions():
    assert fn.present(dict(sqrt=5.0)) == "sqrt( 5.0 )"
    assert fn.present(dict(log=[9,3])) == "log( 9, 3 )"

def test_present_operators():
    assert fn.present(dict(sum=[1,3, -1])) == "1 + 3 + -1"
    assert fn.present(dict(prod=[3,dict(sum=[4,2])])) == "3 * (4 + 2)"
    # TODO strip brackets based on parent and operator relative precedence 
    # assert fn.present(dict(sum=[3,dict(prod=[4,2])])) == "3 + 4 * 2"
