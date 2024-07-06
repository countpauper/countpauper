import pygame
from hex_grid import HexGrid
import argparse
import yaml

import webcolors

class DataModel:
    def __init__(self, file=None):
        if file:
            file_data = yaml.safe_load(file)
        else:
            file_data = dict()

        self.tables = file_data.get("tables", [])
        self.grid = DataModel.parse_grid(file_data.get("grid", dict()))

    
    @staticmethod
    def parse_grid(grid):
        for k, v in grid.items():
            v.update(dict(name=k, 
                          coord=tuple(v.get("coord", ())),
                          color=tuple(webcolors.name_to_rgb(color)) if (color:=v.get("color")) else None
                          ))
        return grid


    def __getitem__(self, coord):
        name, props = ([(name, props) for name, props in self.grid.items() if props.get('coord') == coord] + [(None, dict())])[0]
        if name is None:
            raise IndexError(f"Grid at {coord} is not in the data model")
        props.update(dict(name=name))
        return props


    def colors(self, alpha=255):
        return {grid["coord"]: (*color, alpha) for grid in self.grid.values() if (color:=grid.get("color"))}


# define a main function
def main(*, background="", zoom=1.0, size=(4,3), offset=(0,0), color=(255,255,255), line_width=1, data=None, **args):
    # TODO more more grid properties (hue to fill the hex with alpha)
    # TODO: select the hex and redraw with a thicker outline? 
    # TODO: render description on screen 
    # TODO: yaml extend with random tables
    # TODO: location of party is left click and information right separate events (random table) to move into from peeking ahead
    # TODO: fog of war blacks out unknown hexes 

    grid_data = DataModel(data)

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
                       
    # colorize_grid(grid, grid_data["grids"])
    offset=pygame.Vector2(offset) * zoom
    screen_rect = (offset, pygame.Vector2(screen.size)-offset)
    grid.draw(screen.subsurface(screen_rect), color, line_width, grid_data.colors(50))
    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                exit(0)
            elif event.type == pygame.MOUSEBUTTONUP:
                pos = pygame.mouse.get_pos() - offset
                scale = grid.scale(screen)
                hex_pos = pygame.Vector2(pos[0]/scale, pos[1]/scale)
                if hex_coord := (grid.pick(hex_pos)+[None])[0]:
                    try:
                        data_grid = grid_data[hex_coord]
                    except IndexError:
                        data_grid = None
                else:
                    data_grid = None
                print(f"Click at {pos} = {hex_pos}, {hex_coord} {data_grid}") 
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
    parser.add_argument("-d", "--data", default=None, required=False, type=argparse.FileType('r'),
                        help="A yaml file containg descriptions for each grid, random tables and more")
    
    # color of the hex grid perhaps line witdh where 0 = invisible as well 
    parser.add_argument("-o","--offset", type=coord_argument, required=False, default=(0,0),
                        help="The offset with the upper left corner of the image to draw the first hex on. A small corner will be exluded")
    main(**vars(parser.parse_args()))
