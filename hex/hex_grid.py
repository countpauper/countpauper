import math
import pygame
from shape import hexagon

class HexGrid:
    def _make(horizontal):
        hex = hexagon()
        if horizontal:
            hex = hex.counter_clockwise()
        return (hex + (1,1)) * (0.5, 0.5)

    def __init__(self, grids, horizontal=False):
        """A hexgrid consists of a set of indexed shapes. 
        grids: the width and height in hexagons. 
        NB: Every uneven row (if vertical) or column(if horizontal) has one less
            i,e. a 4x3 vertical hex grid will be 3 rows of 4,3,4 
        horizontal: the sharp end of the hexagon is along the X-axis. By default it's along the Y-axis."""

        line_step = pygame.Vector2(math.sqrt(3)*0.5, 0.75) 
        if horizontal:
            line_step = line_step[1], line_step[0]
        # total size of the hex grid in grids, assuming one vertical hex is 1.0 height
        self.required_size = line_step[0] * grids[0], line_step[1] * grids[1]
        
        # hexagon with a bounding circle of 1.0 has its flattened side 1-sqrt(3)/2 inside the radius. 
        # This offset puts the vertical (sharp side up) hexagon flush against the left edge of the surface
        off_center = -(1-0.5 * math.sqrt(3))
        # Because the hexagon is rescaled from a 1.0 radius -1...+1 circle to 0.5 radius (0...1) this offset has to be halved as well 
        off_center/=2 
        offset = off_center, off_center + line_step[horizontal]/2.0

        self.hex = dict()
        if horizontal:
            for grid_x in range(0, grids[0]):
                for grid_y in range(0, grids[1] - (1 if grid_x&1 else 0)):
                    x = grid_x * line_step[0]
                    y = (offset[grid_x & 1] + grid_y * line_step[1])
                    self.hex[(grid_x, grid_y)] = HexGrid._make(horizontal) + (x,y)
        else:
            for grid_y in range(0, grids[1]): 
                for grid_x in range(0, grids[0]  - (1 if grid_y&1 else 0)):      
                    x = (offset[grid_y & 1] + grid_x * line_step[0])
                    y = grid_y * line_step[1]
                    self.hex[(grid_x, grid_y)] = HexGrid._make(horizontal) + (x,y)

    def pick(self, pos):
        return [coord for coord, hex in self.hex.items() if hex.inside(pos)]    
    
    def scale(self, surface):
        size=surface.get_size()
        return min(size[0]/self.required_size[0], size[1]/self.required_size[1])

    def draw(self, surface, color):
        scale = self.scale(surface)
        for h in self.hex.values():
            h  *= scale
            h.draw(surface, color)
