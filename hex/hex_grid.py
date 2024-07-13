import math
import pygame
from shape import hexagon
from matrix import Matrix2

def render_text(surface, pos, text, size=24, color=(255,255,255), halign=-1, valign=-1):
    """Render some text in the system font
    surface: surface to render on 
    pos: position of the text, The position further depends on the alignment 
    size: Font size 
    color: color to render the font in
    halign: horizontal align. -1: left (default) 0: center, 1:right
    valign: vertical align: -1: top(default), 0: center, 1: bottom
    When top left aligned the position is the top left corner of the font, 
    When center aligned the position is the center of the text."""
    font = pygame.font.SysFont(None, size)
    metrics = font.metrics(text)
    width = sum(m[-1] for m in metrics)
    align_offset=pygame.Vector2((halign*-0.5 - 0.5) * width, 
                                (valign*-0.5 - 0.5) * font.get_height())
    img = font.render(text, True, color)
    surface.blit(img, pos+align_offset)

class Hex:
    def __init__(self, horizontal, offset):
        if horizontal:
            self.shape = hexagon(math.pi * -0.5) 
        else:
            self.shape = hexagon(0)
        self.shape = self.shape * Matrix2.translate(1, 1) * Matrix2.scale(0.5) * Matrix2.translate(*offset)
        self.tool_tip = None
        self.line_color = (255,255,255)
        self.line_width = 1
        self.fill_color = None

    def draw(self, surface, scale):
        shape = self.shape * scale
        shape.draw(surface, self.line_color, self.line_width, self.fill_color)
        if self.tool_tip:
            tip_pos = shape.center() + pygame.Vector2(0, -0.25*scale) # position title in top triangle (if vertical)
            render_text(surface, tip_pos, self.tool_tip, halign=0, valign=0)

class HexGrid:

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
                    self.hex[(grid_x, grid_y)] = Hex(horizontal, (x,y))
        else:
            for grid_y in range(0, grids[1]): 
                for grid_x in range(0, grids[0]  - (1 if grid_y&1 else 0)):      
                    x = (offset[grid_y & 1] + grid_x * line_step[0])
                    y = grid_y * line_step[1]
                    self.hex[(grid_x, grid_y)] = Hex(horizontal, (x,y))

    def pick(self, pos):
        return [coord for coord, hex in self.hex.items() if hex.shape.inside(pos)]    
    
    def scale(self, surface):
        size=surface.get_size()
        return min(size[0]/self.required_size[0], size[1]/self.required_size[1])
    
    def __getitem__(self, idx):
        if type(idx) is int:
            return list(self.hex.values())[idx]
        else:
            return self.hex[idx]

    def draw(self, surface):
        scale = self.scale(surface)
        for c, h in self.hex.items():
            h.draw(surface, scale)
