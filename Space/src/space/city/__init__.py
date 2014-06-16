import structure 

class AvailabilityException(Exception):
    pass

class Agent(object):
    def __init__(self, city, profession):
        self.city = city
        self.profession = profession
        self.busy = 0
        self.happiness = 0
        self.money = 0

    def tick(self, ticks):
        print "{} busy {}/{}".format(self.profession.name, self.busy, self.total)

    def hire(self,amount):
        if amount<idle():
            self.busy += amount
        else:
            raise AvailabilityException("Not enough {0} workers available", self )

class Individual(Agent):
    def __init__(self, city, profession):
        Agent.__init__(self, city, profession)
        self.total = 1

class WorkForce(Agent):
    def __init__(self, city, profession, total=0):
        Agent.__init__(self, city, profession)
        self.total = total




class Population(object):
    def __init__(self):
        self.workers = []

    def tick(self, ticks):
        for work in self.workers:
            work.tick(ticks)

class Order(object):
    def __init__(self, process, bulk, repeat=1):
        self.process = process
        self.timer = 0
        self.repeat = 0
        self.bulk = 0
        self.materials = []

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

class ProductionException(Exception):
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
        raise structure.StorageException("No storage space for item {1} in {0}", self, item)

    def stock(self, product):
        total = 0
        for structure in self.infra:
            total += structure.stock(product)

    def retrieve(self, product):
        for struct in self.infra:
            try:
                return struct.retrieve(product)
            except structure.StorageException:
                pass
        raise structure.StorageException("Item {1} not stored in {0}", self, item)

    def hire(self, profession, amount):
        for worker in self.workers:
            if worker.profession == profession:
                worker.hire(amount) 
                return
        else:
            raise EmploymentException("No {1} workers available in {0}", self, profession)

    def rent(self, building, amount):
        for struct in self.infra:
            if isinstance(struct, building):
                if self.available()<amount:
                    raise 
    def order(self, process, amount):
        order = Order(process, amount)  # TODO split over bulk/repeat based on availability of space/workers

        materials = process.materials
        for material in materials:
            if self.stock(material)<materials[material] * amount:
                raise ProductionError("Not enough {0} for {1}", material, process)
        
        # availability checked, claim
        self.hire(process.profession, amount)
        self.government(process.place, amount)
        for material in materials:
            order.materials.append(self.retrieve(material,materials[material] * amount))
       

class City(Habitation):
    pass

class Base(Habitation):
    pass

class Station(Habitation):
    pass
