import pytest 
from matrix import Matrix2
from pygame import Vector2
import math

def test_identity():
    m1 = Matrix2.identity()
    m2 = Matrix2()
    assert m1==m2

    assert m1.translation() == Vector2(0,0)
    assert m1.scaling() == Vector2(1,1)
    assert m1.rotation() == 0

    v = Vector2(1,2)
    assert m1* v  == v

def test_translation():
    m = Matrix2.translate(1,2)
    assert m.translation() == Vector2(1,2)

    v = Vector2(-3,1)
    assert m*v == Vector2(-2, 3)
    
def test_scale_uniform():
    m = Matrix2.scale(3)
    assert m.scaling() == Vector2(3,3)

    v = Vector2(1,2)
    assert m*v == Vector2(3,6)
    
def test_scale():
    m = Matrix2.scale(2,4)
    assert m.scaling() == Vector2(2,4)

    v = Vector2(1,2)
    assert m*v == Vector2(2,8)

def test_rotation():
    m = Matrix2.rotate(math.pi/2)
    assert m.rotation() == pytest.approx(math.pi/2)

    v=Vector2(1,2)
    assert m*v == Vector2(-2, 1)

def test_clock_rotation():
    assert Matrix2.clockwise().rotation() == pytest.approx(math.pi * -0.5)
    assert Matrix2.counter_clockwise().rotation() == pytest.approx(math.pi * 0.5)

def test_skew():
    assert Matrix2.skew(0.2, 0).skewed() == Vector2(0.2, 0)
    assert Matrix2.skew(0, -0.3).skewed() == Vector2(0, -0.3)
    assert Matrix2.skew(-0.1, 1).skewed() == Vector2(-0.1, 1)
    m = Matrix2.skew(0.2)
    assert m*Vector2(1,1) == Vector2(1 + math.tan(0.2), 1)

def test_flip():
    mh = Matrix2.flip(horizontal=True)
    assert mh*Vector2(1, 2) == Vector2(-1, 2)
    mv = Matrix2.flip(vertical=True)
    assert mv*Vector2(1, 2) == Vector2(1, -2)


def test_multiply_matrix():
    mt = Matrix2.translate(2, 1)
    mr = Matrix2.rotate(math.pi/2)
    # first translate in identity, then rotate. translation stays the same 
    left_mul = mt * mr 
    assert left_mul.coef.shape == (3,3)
    assert left_mul.translation() == Vector2(2,1)
    assert left_mul.rotation() == math.pi / 2

    right_mul = mr * mt
    # first rotate, then translate in the new orientation: translation is also rotated
    assert right_mul.translation() == Vector2(-1, 2)
    assert right_mul.rotation() == math.pi / 2

def test_matrix_inverse():
    m = Matrix2.translate(1,2).inverse()
    assert m.translation() == Vector2(-1,-2)
    m = Matrix2.rotate(0.8).inverse()
    assert m.rotation() == -0.8

