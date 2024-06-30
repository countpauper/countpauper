import pygame 
import math

class Shape:
    def __init__(self, points):
        self.points = points
            
    def translate(self, t):
        # TODO __add__
        return Shape([pygame.Vector2(p) + pygame.Vector2(t) for p in self.points])

    def __add__(self, t):
        return self.translate(t)
    
    def scale(self, s):
        s= pygame.Vector2(s)
        return Shape([pygame.Vector2(p[0] * s[0], p[1] * s[1]) for p in self.points])

    def __mul__(self, s):
        return self.scale(s)
    
    def rotate(self, a):
        return Shape([pygame.Vector2(p).rotate_rad(a) for p in self.points])

    def clockwise(self):
        return self.rotate(math.pi * -0.5)

    def counter_clockwise(self):
        return self.rotate(math.pi * 0.5)

    def flip(self):
        return self.rotate(math.pi)

    def project(self, plane):
        """Return the projection of a point on a plane"""
        pass

    def plane(self):
        """Return the normal vector and offset of all pointsr in the shape, assuming they are coplanar"""
        pass

    def inside(self, point):
        """ Return whether the project of a point on the plane of the shape is "inside" a counter clockwise convex 2d shape"""
        # TODO: dunder for in
        # TODO dot products all negative = inside 
        pass 

    def draw(self, surface, outline_color, line_width=1):
        pygame.draw.polygon(surface, outline_color, self.points, line_width)

    @classmethod
    def regular_polygon(c, n):
        """Create a polygon in the XY plane, descibred as an n by 3 matrix, with equal sides and angles for each edge.
        The polygon starts at angle 0, is counter clockwise, and it fits inside a bounding circle with radius 1 centered at 0,0"""
        return Shape([pygame.Vector2(math.sin(2*math.pi * i / float(n)), math.cos(2*math.pi * i / float(n))) for i in range(n)])

def hexagon():
    return Shape.regular_polygon(6)

