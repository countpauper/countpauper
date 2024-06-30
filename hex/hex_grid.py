import math
import pygame
from shape import hexagon

class HexGrid:
    def __init__(self, grids, horizontal=False):
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

        for grid_x in range(0, grids[0]):       # TODO one less if horizontal and even
            y=0
            for grid_y in range(0, grids[1]):   # TODO less if vertical and even 
                if horizontal:
                    x = grid_x * line_step[0]
                    y = (offset[grid_x & 1] + grid_y * line_step[1])
                else:
                    x = (offset[grid_y & 1] + grid_x * line_step[0])
                    y = grid_y * line_step[1]

                hex = hexagon()
                if horizontal:
                    hex = hex.counter_clockwise()
                hex = (hex + (1,1)) * (0.5, 0.5)
                transformed_polygon = hex + (x,y)
                self.hex[(grid_x, grid_y)] = transformed_polygon

    def draw(self, surface, color):
        size=surface.get_size()
        scale = min(size[0]/self.required_size[0], size[1]/self.required_size[1])
        for h in self.hex.values():
            h  = h * scale
            h.draw(surface, color)
