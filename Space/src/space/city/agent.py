from space.item import Food
from space.time import Period
from space.city.market import PriceException

import space.constants as constants

class EmploymentException(Exception):
    pass

class Agent(object):
    def __init__(self, occupation):
        self.occupation = occupation
        self.busy = 0
        self.money = 0
        self.hunger = 0 
        self.food_offer = None

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
        if self.food_offer:
            food = self.food_offer.item
            food.consume(self) # TODO: clean up or reuse offer
        options = location.shop(Food)
        for product, amount in options.iteritems():
            try:
                if amount >= self.total:    # need enough food for everyone 
                    self.food_offer = location.buy(self, product, self.total, self.hunger)
            except PriceException:
                pass

class Individual(Agent):
    def __init__(self, occupation):
        Agent.__init__(self, occupation)
        self.total = 1

class Group(Agent):
    def __init__(self, occupation, total=0):
        Agent.__init__(self, occupation)
        self.total = total
