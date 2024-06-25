import pygame
import math
# import numpy    # TODO use numpy or pygame.math

def regular_polygon(n):
    """Create a polygon in the XY plane, descibred as an n by 3 matrix, with equal sides and angles for each edge.
    The polygon starts at angle 0, is counter clockwise, and it fits inside a bounding circle with radius 1 centered at 0,0"""
    return [(math.sin(2*math.pi * i / float(n)), math.cos(2*math.pi * i / float(n))) for i in range(n)]

def hexagon():
    hex = regular_polygon(6)
    return hex
    # return [(0.5,0),
    #         (1.0,0.25),
    #         (1.0,0.75),
    #         (0.5,1.0),
    #         (0.0,0.75),
    #         (0,0.25)]

def translate(shape, t):
    x,y=t
    return [(x + px , y + py) for px,py in shape]

def scale(shape, s):
    w, h=s
    return [(px*w, py*h) for px,py in shape]

def rotate(points, a):
    pass


def project(plane, point):
    """Return the projection of a point on a plane"""
    pass

def plane(shape):
    """Return the normal vector and offset of all pointsr in the shape, assuming they are coplanar"""
    pass

def inside(shape, point):
    """ Return whether the project of a point on the plane of the shape is "inside" a counter clockwise convex 2d shape"""
    # TODO dot products all negative = inside 
    pass 

def draw_hexagon(surface, boundingbox):
    hex = scale(translate(hexagon(), (1,1)), (0.5,0.5))
    x, y, w, h = boundingbox
    transformed_polygon = translate(scale(hex, (w,h)), (x,y))
    pygame.draw.polygon(surface,(255,255,255),transformed_polygon,1)
                

def draw_hex_grid(surface, grids, horizontal=False):
    size=surface.get_size()
    line_step = (0.75, math.sqrt(3)*0.5) if horizontal else (math.sqrt(3)*0.5, 0.75) 
    required_size=line_step[0] * grids[0], line_step[1] * grids[1]
    scale = min(size[0]/required_size[0], size[1]/required_size[1])
    # hexagon with a bounding circle of 1.0 has its flattened side 1-sqrt(3)/2 inside the radius. 
    # This offset puts the vertical (sharp side up) hexagon flush against the left edge of the surface
    off_center = -(1-0.5 * math.sqrt(3))
    # Because the hexagon is rescaled from a 1.0 radius -1...+1 circle to 0.5 radius (0...1) this offset has to be halved as well 
    off_center/=2 
    offset = off_center, off_center + line_step[0]/2.0

    for grid_x in range(0,grids[0]):
        for grid_y in range(0, grids[1]):
            # TODO: reverse if horizontal
            x = (offset[grid_y & 1] + grid_x * line_step[0]) * scale
            y = grid_y * line_step[1] * scale
            draw_hexagon(surface, pygame.Rect(x, y, scale, scale))
    

# define a main function
def main():
    pygame.init()
    icon = pygame.image.load("hex.jpg")
    pygame.display.set_icon(icon)
    pygame.display.set_caption("Hex")

    screen = pygame.display.set_mode((1024,768)) 
    running = True
    white = (255,255,255)
    background_color = (100,50,0)
    screen.fill(background_color)
    draw_hex_grid(screen, (4,3))

    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
        pygame.display.update()
if __name__=="__main__":
    main()
