import pytest 
from shape import Shape, Edge
from matrix import Matrix2
import pygame

def test_point():
    s = Shape((1,2))
    assert len(s) == 1
    assert s[0] == pygame.Vector2(1,2)
    assert not s.is_closed()
    assert (1,2) in s

def test_edge():
    s = Edge((1,2),(3,4))
    assert len(s) == 2
    assert s[1] == pygame.Vector2(3,4)

def test_on_edge():
    s = Edge((1,2),(3,4))
    assert not s.is_closed()
    assert s[0] in s
    assert s[1] in s
    assert s.center() in s
    assert (2.1, 3) not in s
    assert s[1] + s.vector()*0.1 not in s
    assert s[0] - s.vector()*0.001 not in s

def test_transform():
    s = Shape((1,2), (2,3))
    s *= Matrix2.translate(1, 1)
    assert s[0] == pygame.Vector2(2,3)
    assert s[1] == pygame.Vector2(3,4)
    
def test_regular_triangle():
    s = Shape.regular_polygon(3)
    assert len(s) == 3
    assert s.center() == pygame.Vector2(0, 0)
    assert pygame.Vector2(0.5, 0) in s

def test_regular_diamond():
    s = Shape.regular_polygon(4)
    assert s.boundingbox() == ((-1,-1), (2,2))
    assert pygame.Vector2(1,1) not in s