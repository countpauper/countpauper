class EmploymentException(Exception):
    pass

class _Agent(object):
    def __init__(self, city, occupation):
        self.city = city
        self.occupation = occupation
        self.busy = 0
        self.happiness = 0
        self.money = 0

    def tick(self, ticks):
        print "{} busy {}/{}".format(self.occupation.name, self.busy, self.total)

    def idle(self):
        return self.total-self.busy

    def hire(self,amount):
        if amount<=self.idle():
            self.busy += amount
        else:
            raise EmploymentException("Not enough {0} workers available", self )

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

    def tick(self, ticks):
        for work in self.workers:
            work.tick(ticks)
