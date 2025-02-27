class AABB:
    """Axis aligned bounding box"""
    def __init__(self, bounds):
        self.bounds = tuple(bounds) # assumed to be a tuple of ((minx, maxx), (miny,maxy), (minz,maxz)) for now

    def inside(self, coords):
        return all(
            v>=limit[0] and v<=limit[1] 
            for v,limit in zip(coords, self.bounds))
    
    def __repr__(self):
        return ",".join(f"({lo}…{hi})" for lo, hi in self.bounds)