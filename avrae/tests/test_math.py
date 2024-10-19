import pytest
from gvar import import_gvar

math=import_gvar("math/5f72a0b9-57c3-4223-a7a4-272e57b9bfc3.gvar", "math")

def test_nan():
    assert math.isnan(math.nan)
    assert not math.isnan(1)

def test_isclose():
    assert math.isclose(1, 1)
    assert math.isclose(1, 1.0000000000001)
    assert not math.isclose(-2, -2.001)
    assert math.isclose(3, 3.001, 1e-3)

def test_sqrt():
    assert math.sqrt2(4) == 2
    assert math.sqrt2(0) == 0
    assert math.isnan(math.sqrt2(-1))

def test_prod():
    assert math.prod() == 1
    assert math.prod(5) == 5 
    assert math.prod(2,3) == 6
    assert math.prod(2,3,4) == 24

def test_pow():
    assert math.pow() == 1
    assert math.pow(5) == 5
    assert math.pow(5, 2) == 25
    assert math.pow(3, 2, 2) == 81

def test_factorial():
    assert math.factorial(0) == 1
    assert math.factorial(1) == 1
    assert math.factorial(3) == 6
    assert math.factorial(3.2) == 6

def test_comb():
    assert math.comb(0, 0) == 1
    assert math.comb(1, 2) == 0
    assert math.comb(2, 1) == 2
    assert math.comb(10, 5) == 252
    assert math.comb(1000, 3) == 166167000
    with pytest.raises(RuntimeError):
        math.comb(-1, 2)
    with pytest.raises(RuntimeError):
        math.comb(2, -1)
    
def test_ln():
    assert math.isnan(math.log(0))
    assert math.isnan(math.log(-1))
    assert math.log(1) == 0
    print(math.log(2))
    assert math.isclose(math.log(math.e), 1)
    assert math.isclose(math.log(100), 4.605170185988092)
    assert math.isclose(math.log(9), 2.1972245773362196, 0.001)
    assert math.isclose(math.log(0.1), -2.3025850929940455)
    assert math.isclose(math.log(0.04321), -3.1416833290382953)    
    assert math.isclose(math.log(12345), 9.42100640177928)

def test_log():
    assert math.log(100,10) == 2
    assert math.isclose(math.log(64,2), 6)

def test_degrees():
    assert math.degrees(math.pi) == 180
    assert math.degrees(0) == 0
    assert math.degrees(-math.pi/2) == -90

def test_radians():
    assert math.radians(180) == math.pi 
    assert math.radians(0) == 0
    assert math.radians(-360) == -2*math.pi

def test_sin():
    assert math.sin(0) == 0
    assert math.sin(math.pi * 0.5) == 1
    assert math.isclose(math.sin(-math.pi), 0, abs_tol=1e-9)
    assert math.isclose(math.sin(-1.0), -0.8414709848078965)
    assert math.isclose(math.sin(1000.0), 0.8268795405320025)

def test_cos():
    assert math.cos(0) == 1
    assert math.isclose(math.cos(math.pi * 0.5), 0, abs_tol=1e-9)
    assert math.isclose(math.cos(-math.pi), -1)
    assert math.isclose(math.cos(-1.0), 0.5403023058681398)
    assert math.isclose(math.cos(2000.0), -0.36745954910083134)

def test_tan():
    assert math.tan(0) == 0
    assert math.isclose(math.tan(1),1.5574077246549023)
    assert math.isclose(math.tan(-2), 2.185039863261519)

def test_atan():
    assert math.atan(0) == 0
    assert math.isclose(math.atan(1), math.pi/4, rel_tol=1e-9)
    assert math.isclose(math.atan(-4), -1.3258176636680326, rel_tol=1e-9)
    assert math.isclose(math.atan(10e9), math.pi/2, rel_tol=1e-9)

def test_atan2():
    assert math.atan2(0,  1) == 0
    assert math.atan2(0,  -1) == math.pi
    assert math.atan2(1,  0) == math.pi / 2
    assert math.atan2(-1, 0) == -math.pi / 2
    assert math.isnan(math.atan2(0, 0))
    assert math.isclose(math.atan2(1, 1), math.pi / 4)
    assert math.isclose(math.atan2(-1, 1), -math.pi/ 4)
    assert math.isclose(math.atan2(1, -1), 3 * math.pi / 4)
    assert math.isclose(math.atan2(-1, -1), -3* math.pi/ 4)
    assert math.isclose(math.atan2(12, -34), 2.8023000391357487)

def test_dist():
    assert math.dist((),()) == 0
    assert math.dist((0,),(0,)) == 0
    assert math.dist((1, 0), [2, 0]) == 1
    assert math.isclose(math.dist((0, 1, 0), (2, 3, -4)),4.898979485566356)