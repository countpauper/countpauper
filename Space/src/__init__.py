class WorkForce(object):
    def __init__(self, city, type, total=0):
        self.city = city
        self.type = type
        self.total = total
        self.busy = 0
 
   
    def tick(self, ticks):
        print "{} busy {}/{}".format(self.type.name, self.busy, self.total)

class StorageException(Exception):
    pass

class Item(object):
    def __init__(self, type, amount):
        self.type = type
        self.amount = amount

    def volume(self):
        return self.type.volume * amount

class Population(object):
    def __init__(self):
        self.workers = []
        self.happiness = 0

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


class City(object):
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
        for structure in self.infra:
            try:
                structure.store(item)
                return
            except StorageException:
                pass
        raise StorageException("No storage for item", item)

    def stock(self, product):
        total = 0
        for structure in self.infra:
            total += structure.stock(product)

