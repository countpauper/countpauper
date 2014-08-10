from space.item import Food
from space.time import Period
from space.city.market.exception import PriceException
from specification import Resource

import space.constants as constants

class EmploymentException(Exception):
    pass

class Agent(Resource):  # TODO own resource, own organization? or org is an agent? (can buy company)
    def __init__(self, specification, amount):
        super(Agent, self).__init__(specification, amount)
        self.busy = 0
        self.money = 0
        self.hunger = 0 
        self.groceries = None
        self.job = None

    def tick(self, location, time, period):
        if not self.job:
            self.job = location.sell(self, self, 100)
        else:
            # todo: base price of costs:
            #   food, unemployment rate
            #   later: rent, tax, loans, luxury
            # if price not met, fire people for rate 
            #  or lower luxury
            # if unemployment low, train people
            # if price met, raise luxury level
            if self.busy < self.amount:
                self.job.adjust(self.job.price * 0.9)
            else:
                self.job.adjust(self.job.price * 1.1)

        self.hunger += period.days() * constants.calories_per_day
        if self.hunger>=constants.calories_per_meal:  # TODO:just schedule meals in scheduler per 8 hours
            self.eat(location)
            # print "{} eating {}".format(time, self)

    def happiness(self):
        score = 0
        score += min(0,self.hunger) # TODO hunger factor
        return score

    def idle(self):
        return self.amount-self.busy

    def purchase(self, amount):
        if amount<=self.idle():
            self.busy += amount
            return self
        else:
            raise EmploymentException("Not enough {0} workers available", self )

    def fire(self,amount):
        if amount<=self.busy:
            self.busy -= amount
        else:
            raise EmploymentException("Not enough {0} workers hired", self )

    def eat(self, location):
        if self.groceries:
            food = self.groceries.claim()
            food.consume(self) 
            self.groceries = None
        options = location.shop(Food)
        for product, amount in options.iteritems():
            try:
                if amount >= self.amount:    # need enough food for everyone 
                    self.groceries = location.buy(self, product, self.amount, self.hunger)
            except PriceException:
                pass

class Individual(Agent):
    def __init__(self, specification):
        super(Individual, self).__init__(self, specification,1)

class Population(Agent):
    pass