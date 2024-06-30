import pygame
import math
from shape import hexagon

def draw_hexagon(surface, boundingbox, horizontal=False):
    hex = hexagon()
    if horizontal:
        hex = hex.counter_clockwise()
    hex = (hex + (1,1)) * (0.5, 0.5)

    x, y, w, h = boundingbox
    transformed_polygon = hex * (w,h) + (x,y)
    transformed_polygon.draw(surface, (255,255,255))
                

def draw_hex_grid(surface, grids, horizontal=False):
    size=surface.get_size()
    line_step = pygame.Vector2(0.75, math.sqrt(3)*0.5) if horizontal else (math.sqrt(3)*0.5, 0.75) 
    required_size = line_step[0] * grids[0], line_step[1] * grids[1]
    scale = min(size[0]/required_size[0], size[1]/required_size[1])
    # hexagon with a bounding circle of 1.0 has its flattened side 1-sqrt(3)/2 inside the radius. 
    # This offset puts the vertical (sharp side up) hexagon flush against the left edge of the surface
    off_center = -(1-0.5 * math.sqrt(3))
    # Because the hexagon is rescaled from a 1.0 radius -1...+1 circle to 0.5 radius (0...1) this offset has to be halved as well 
    off_center/=2 
    offset = off_center, off_center + line_step[horizontal]/2.0

    for grid_x in range(0, grids[0]):       # TODO one less if horizontal and even
        for grid_y in range(0, grids[1]):   # TODO less if vertical and even 
            if horizontal:
                x = grid_x * line_step[0] * scale
                y = (offset[grid_x & 1] + grid_y * line_step[1]) * scale
            else:
                x = (offset[grid_y & 1] + grid_x * line_step[0]) * scale
                y = grid_y * line_step[1] * scale
            draw_hexagon(surface, pygame.Rect(x, y, scale, scale), horizontal)
    

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
    draw_hex_grid(screen, (4,3), True)

    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
        pygame.display.update()

if __name__=="__main__":
    main()
