from space.item import Food
from structure import StorageException
from space.time import Period
import space.constants as constants

class EmploymentException(Exception):
    pass

class Agent(object):
    def __init__(self, occupation):
        self.occupation = occupation
        self.busy = 0
        self.money = 0
        self.hunger = 0 

    def tick(self, location, time, period):
        self.hunger += period.days() * constants.calories_per_day
        if self.hunger>=constants.calories_per_meal:  # TODO:just schedule meals in scheduler per 8 hours
            self.eat(location)
            # print "{} eating {}".format(time, self)

    def __repr__(self):
        return "{} x {}".format(self.total, self.occupation)

    def happiness(self):
        score = 0
        score += min(0,self.hunger) # TODO hunger factor
        return score

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

    def eat(self, location):
        try:
            food = location.retrieve(Food, self.total)
            food.consume(self)
        except StorageException:
            pass

class Individual(Agent):
    def __init__(self, occupation):
        Agent.__init__(self, occupation)
        self.total = 1

class Group(Agent):
    def __init__(self, occupation, total=0):
        Agent.__init__(self, occupation)
        self.total = total
