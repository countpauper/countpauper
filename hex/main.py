import pygame
from hex_grid import HexGrid
import argparse

# define a main function
def main(*, background="", zoom=1.0, size=(4,3), offset=(0,0), color=(255,255,255), line_width=1, **args):
    # TODO: argparser: background image and size
    # TODO: json or something contains name, description, random tables and so on (hue) at hex
    # TODO: fog of war ? 

    pygame.init()
    icon = pygame.image.load("hex.jpg")
    pygame.display.set_icon(icon)
    pygame.display.set_caption("Hex")

    bg_image = pygame.image.load(background)
    if zoom!= 1.0:
        scaled_bg = pygame.transform.smoothscale(bg_image, pygame.Vector2(bg_image.size) * zoom)
    else:
        scaled_bg = bg_image
    screen = pygame.display.set_mode(scaled_bg.size) 
    screen.blit(scaled_bg,((0,0),(screen.size)))

    grid = HexGrid(size, False)
    offset=pygame.Vector2(offset) * zoom
    screen_rect = (offset, pygame.Vector2(screen.size)-offset)
    grid.draw(screen.subsurface(screen_rect), color, line_width)
    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                exit(0)
            elif event.type == pygame.MOUSEBUTTONUP:
                pos = pygame.mouse.get_pos()
                scale = grid.scale(screen)
                hex_pos = pygame.Vector2(pos[0]/scale, pos[1]/scale)
                hex_coord = grid.pick(hex_pos)
                print(f"Click at {pos} = {hex_pos}, {hex_coord}") 
        pygame.display.update()

def vector_argument(arg, splitter=",",  dim=2, bounds=None):
    result = arg.split(splitter)
    if len(result)!=dim:
        raise argparse.ArgumentError(f"Faulty vector format. {arg} should {dim} scalars separated by '{splitter}'")
    result = tuple(int(scalar) for scalar in result)
    if bounds is not None and any(s not in bounds for s in result):
        raise argparse.ArgumentError(f"Faulty vector argument {arg}. Each scalar should be within bounds.")
    return result 

def coord_argument(arg):
    return vector_argument(arg)

def size_argument(arg):
    return vector_argument(arg, "x", bounds=range(0,1000))

def color_argument(arg):
    return vector_argument(arg, bounds=range(0,256), dim=3)

if __name__=="__main__":
    parser = argparse.ArgumentParser(
                    prog="hex grid",
                    description='Hex grid over a map, with info')
    parser.add_argument("--background", "-bg", default="background.png",type=argparse.FileType('rb'),
                        help="The background image containing the map to draw over. The window will adjust to the image size")
    parser.add_argument("-s","--size", type=size_argument, required=True,
                            help="the number of horizontal and vertical grids.")
    parser.add_argument("-z", "--zoom", default=1.0, required=False, type=float,
                            help="Zoom factor to fit the image and the grid on the screen.")
    parser.add_argument("-c", "--color", default=(255,255,255), required=False, type=color_argument,
                            help="Color of the hex grid as (R,G,B) with each channel 0-255")
    parser.add_argument("-l", "--line_width", default=1, required=False, type=int,
                        help="Line width of the hex grid")
    
    # color of the hex grid perhaps line witdh where 0 = invisible as well 
    parser.add_argument("-o","--offset", type=coord_argument, required=False, default=(0,0),
                        help="The offset with the upper left corner of the image to draw the first hex on. A small corner will be exluded")
    main(**vars(parser.parse_args()))
