import structure 

class WorkForce(object):
    def __init__(self, city, profession, total=0):
        self.city = city
        self.profession = profession
        self.total = total
        self.busy = 0
        self.happiness = 0
        self.money = 0
 
    def tick(self, ticks):
        print "{} busy {}/{}".format(self.profession.name, self.busy, self.total)


class Population(object):
    def __init__(self):
        self.workers = []

    def tick(self, ticks):
        for work in self.workers:
            work.tick(ticks)

class Order(object):
    def __init__(self, owner, process, repeat, bulk):
        self.owner = owner
        self.process = process
        self.timer = 0
        self.repeat = 0
        self.bulk = 0

    def tick(self, ticks):
        self.timer += ticks
        print "Order {}/{}".format(self.timer, self.process.duration)
        if self.duration>=self.process.duration:    # TODO: repeat
            self.done()

    def done(self, ticks):
        product = Item(process.product, bulk)
        self.owner.store(product)
        self.owner.orders.remove(self)

class Government(object):
    def tick(self, ticks):
        pass

class Habitation(object):
    def __init__(self):
        self.government = Government()
        self.population = Population()
        self.orders = []
        self.infra = []

    def tick(self, ticks):
        self.government.tick(ticks)
        self.population.tick(ticks)
        for order in self.orders:
            order.tick(ticks)

    def store(self, item):
        for struct in self.infra:
            try:
                struct.store(item)
                return
            except structure.StorageException:
                pass
        raise structure.StorageException("No storage for item", item)

    def stock(self, product):
        total = 0
        for structure in self.infra:
            total += structure.stock(product)

class City(Habitation):
    pass

class Base(Habitation):
    pass

class Station(Habitation):
    pass
