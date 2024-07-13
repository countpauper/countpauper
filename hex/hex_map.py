import yaml
import webcolors
    
class HexMap:

    class Grid:
        def __init__(self, name, props):
            self.name = name
            self.coord = tuple(props.get("coord", ()))
            self.color = color=tuple(webcolors.name_to_rgb(color)) if (color:=props.get("color")) else None

        def __str__(self):
            return self.name

    def __init__(self, file=None):
        if file:
            self._data = yaml.safe_load(file)
        else:
            self._data = dict()

    @staticmethod
    def parse_grid(name, prop):
        grid = dict(prop)
        grid.update(dict(name=name, 
                          coord=tuple(prop.get("coord", ())),
                          color=tuple(webcolors.name_to_rgb(color)) if (color:=prop.get("color")) else None
                          ))
        return grid

    def _grids(self):
        for name, props in self._data["grid"].items():
            yield HexMap.Grid(name, props)

    def __getitem__(self, coord):
        if grids := [grid for grid in self._grids() if grid.coord == coord]:
            return grids[0]
        else:
            raise IndexError("Grid at {coord} is not loaded")


    def colors(self, alpha=255):
        return {grid.coord: (*grid.color, alpha) for grid in self._grids() if grid.color}
