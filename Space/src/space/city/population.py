from space.item import Food
from structure import StorageException
from space.time import Period

class EmploymentException(Exception):
    pass

class _Agent(object):
    def __init__(self, city, occupation):
        self.city = city
        self.occupation = occupation
        self.busy = 0
        self.happiness = 0
        self.money = 0
        self.hunger = Period(0) # TODO: just schedule meals in scheduler

    def tick(self, time, period):
        self.hunger += period
        print "{} busy {}/{} hungry {} happy {}".format(self.occupation.name, self.busy, self.total, self.hunger, self.happiness)
        if self.hunger>=Period(8):  # TODO: don't hardcode food time, time in Period, not 8 ticks but 8 hours
            self.eat()

    def idle(self):
        return self.total-self.busy

    def hire(self,amount):
        if amount<=self.idle():
            self.busy += amount
        else:
            raise EmploymentException("Not enough {0} workers available", self )

    def fire(self,amount):
        if amount<=self.busy:
            self.busy -= amount
        else:
            raise EmploymentException("Not enough {0} workers hired", self )

    def eat(self):
        try:
            food = self.city.retrieve(Food, self.total)
            food.consume(self)
        except StorageException:
            self.happiness -= 1  # TODO per tick

class Individual(_Agent):
    def __init__(self, city, occupation):
        Agent.__init__(self, city, occupation)
        self.total = 1

class Group(_Agent):
    def __init__(self, city, occupation, total=0):
        _Agent.__init__(self, city, occupation)
        self.total = total

class Population(object):
    def __init__(self):
        self.workers = []

    def tick(self, time, period):
        for work in self.workers:
            work.tick(time, period)
