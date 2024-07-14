import pygame
from hex_grid import HexGrid
from hex_map import HexMap
import argparse

def render(surface, background, grid):
    surface.blit(background,((0,0),(surface.size)))
    screen_rect = (grid.offset, pygame.Vector2(surface.size)-grid.offset)
    grid.draw(surface.subsurface(screen_rect))
    pygame.display.update()


def main(*, background=None, zoom=1.0, size=(4,3), offset=(0,0), color=(255,255,255), line_width=1, data=None, **args):
    # TODO: select the clicked hex and redraw with a thicker outline? 
    # TODO: yaml extend with random tables
    # TODO: location of party is left click and information right separate events (random table) to move into from peeking ahead
    # TODO: fog of war blacks out unknown hexes 
    # TODO: edit box to change the yaml and save it again?
    grid_data = HexMap(data)

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

    grid = HexGrid(size, False)
    for hex in grid:
        hex.line_width = line_width
        hex.line_color = color
    for coord, color in grid_data.colors(50 if background else 255).items():
        grid[coord].fill_color = color

    last_highlighted = None
    grid.offset=pygame.Vector2(offset) * zoom
    render(screen, scaled_bg, grid)

    while True:
        invalidated = True
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                exit(0)
            elif event.type == pygame.MOUSEMOTION:
                pos = pygame.mouse.get_pos() - grid.offset   # TODO let grid use offset and scale, ie give it a matrix
                scale = grid.scale(screen)
                hex_pos = pygame.Vector2(pos[0]/scale, pos[1]/scale)
                hex_coord = (grid.pick(hex_pos)+[None])[0]
                if last_highlighted != hex_coord:
                    invalidated = False
                    if last_highlighted:
                        grid[last_highlighted].tool_tip = None
                    last_highlighted = hex_coord
                    if hex_coord:
                        try:
                            data_grid = grid_data[hex_coord]
                            print(f"At {pos} = {hex_pos} = {hex_coord} {data_grid}")
                            grid[hex_coord].tool_tip = str(data_grid)
                        except IndexError:
                            data_grid = None
                            print(f"At {pos} = {hex_pos} = {hex_coord}") 
                    else:
                        data_grid = None
        
        if invalidated:
            render(screen, scaled_bg, grid)


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
    parser.add_argument("-d", "--data", default=None, required=False, type=argparse.FileType('r'),
                        help="A yaml file containg descriptions for each grid, random tables and more")
    
    # color of the hex grid perhaps line witdh where 0 = invisible as well 
    parser.add_argument("-o","--offset", type=coord_argument, required=False, default=(0,0),
                        help="The offset with the upper left corner of the image to draw the first hex on. A small corner will be exluded")
    main(**vars(parser.parse_args()))
