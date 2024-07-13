import pygame 
import pygame.gfxdraw
import math
from sys import float_info
from matrix import Matrix2

class Edge:
    pass

class Shape:
    def __init__(self, *points):
        self.points = list(pygame.Vector2(p) for p in points)

    def _mul_matrix(self, m):
        return Shape(*[m*p for p in self.points])
 
    def __mul__(self, o):
        if type(o) is Matrix2:
            return self._mul_matrix(o)
        else:
            return self._mul_matrix(Matrix2.scale(o))
    
    def is_closed(self):
        # TODO: could define line shape that is not closed explicitly
        return len(self.points) >= 3
    
    def edges(self):
        return [Edge(p, self.points[(n+1)%len(self.points)]) for n, p in enumerate(self.points)]
                
    def inside(self, point):
        """ Return whether the project of a point on the plane of the shape is "inside" a closed shape.
        This assumes the shape is convex and 2d"""
        if not self.is_closed():
            return any(point in edge for edge in self.edges())
        return all(e.vector().cross(point - e[0]) < 0 for e in self.edges())

    def __contains__(self, point):
        return self.inside(point)

    def __len__(self):
        return len(self.points)
    
    def __getitem__(self, idx):
        return self.points[idx]
    
    def draw(self, surface, outline_color, line_width=1, fill_color=None):
        if surface.get_flags() & pygame.SRCALPHA:
            if line_width > 0:
                pygame.draw.polygon(surface, outline_color, self.points, line_width)
            if self.is_closed() and fill_color:
                pygame.draw.polygon(surface, fill_color, self.points, 0)
        else:
            if line_width:=1:
                pygame.gfxdraw.aapolygon(surface, self.points, outline_color)
            elif line_width > 1:
                assert(len(outline_color)==3)   # TODO gfxdraw doesnt support line width but is needed for alpha on this surface
                pygame.draw.polygon(surface, outline_color, self.points, line_width)
            if self.is_closed() and fill_color:
                pygame.gfxdraw.filled_polygon(surface, self.points, fill_color)


    def boundingbox(self):
        left, top, bottom, right = (min(p[0] for p in self.points), 
                                    min(p[1] for p in self.points),
                                    max(p[0] for p in self.points), 
                                    max(p[1] for p in self.points))
        return ((left, top), (right-left, bottom-top))                
    
    def center(self):
        return (sum(p[0] for p in self.points)/len(self.points), 
                sum(p[1] for p in self.points)/len(self.points))
    
    @staticmethod
    def regular_polygon(n, angle=0):
        """Create a polygon in the XY plane, descibred as an n by 3 matrix, with equal sides and angles for each edge.
        The polygon starts at the give nangle (default 0), is counter clockwise, and it fits inside a bounding circle with radius 1 centered at 0,0"""
        return Shape(*[pygame.Vector2(math.sin(angle + 2*math.pi * i / float(n)), 
                                      math.cos(angle + 2*math.pi * i / float(n))) 
                                      for i in range(n)])


class Edge(Shape):
    def __init__(self, a, b):
        super(Edge, self).__init__(a,b)

    def vector(self):
        return self.points[1] - self.points[0]
    
    def __contains__(self, p):
            vp = p - self.points[0]
            ve = self.vector()
            if math.fabs(ve.cross(vp)) > float_info.epsilon:
                return False
            dot = ve.dot(vp)
            if dot < 0:
                return False
            if dot > ve.length_squared():
                return False
            return True

def hexagon(angle=0):
    return Shape.regular_polygon(6, angle)

