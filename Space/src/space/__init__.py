import city
import item

class Space(object):
    def __init__(self):
        self.systems = []

    def tick(self, ticks):
        for system in self.systems:
            system.tick(ticks)

class System(object):
    def __init__(self):
        self.stars = []
        self.name = str(id(self))   # TODO generate funnier unique name

    def tick(self, ticks):
        for star in self.stars:
            star.tick(ticks)

class Orbital(object):
    def __init__(self):
        self.bodies = []
        self.cities = []

    def tick(self, ticks):
        for body in self.bodies:
            body.tick(ticks)
        for city in self.cities:
            city.tick(ticks)

class Planet(Orbital):
    pass

class Star(Orbital):
    pass

class Moon(Orbital):
    pass

class Asteroid(Orbital):
    pass

class Station(Orbital):
    pass
