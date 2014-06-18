import city
import item
from time import Time, Period

class Space(object):
    def __init__(self):
        self.time = Time(0)
        self.systems = []

    def tick(self, ticks):
        period = Period(ticks)
        print "Time: {}".format(self.time)
        for system in self.systems:
            system.tick(time, period)
        self.time += period

class System(object):
    def __init__(self):
        self.stars = []
        self.name = str(id(self))   # TODO generate funnier unique name

    def tick(self, time, period):
        for star in self.stars:
            star.tick(time, period)
 
class Orbital(object):
    def __init__(self):
        self.bodies = []
        self.cities = []

    def tick(self, time, period):
        for body in self.bodies:
            body.tick(time, period)
        for city in self.cities:
            city.tick(time, period)

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
