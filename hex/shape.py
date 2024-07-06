import pygame 
import math


def dot(a,b):
    return a[0]*b[1]-a[1]*b[0]

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

    def is_closed(self):
        # TODO: could define line shape that is not closed explicitly
        return len(self.points) >= 3
    

    def inside(self, point):
        """ Return whether the project of a point on the plane of the shape is "inside" a closed shape.
        This assumes the shape is convex and 2d"""
        # TODO: unit test 
        if not self.is_closed():
            return False

        return all(
            (self.points[(n + 1)%len(self.points)] - p).cross(
                point - p) < 0 for n,p in enumerate(self.points))

    def __contains__(self, point):
        return self.inside(point)

    def draw(self, surface, outline_color, line_width=1):
        pygame.draw.polygon(surface, outline_color, self.points, line_width)

    @classmethod
    def regular_polygon(c, n):
        """Create a polygon in the XY plane, descibred as an n by 3 matrix, with equal sides and angles for each edge.
        The polygon starts at angle 0, is counter clockwise, and it fits inside a bounding circle with radius 1 centered at 0,0"""
        return Shape([pygame.Vector2(math.sin(2*math.pi * i / float(n)), math.cos(2*math.pi * i / float(n))) for i in range(n)])

def hexagon():
    return Shape.regular_polygon(6)

