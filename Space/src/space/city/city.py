from population import Population, EmploymentException
from structure import StorageException
from process import Process, ProductionException
from specification import Recipe
from utility.observer import Observer

class Government(object):
    def tick(self, ticks):
        pass

class Habitation(Observer):
    def __init__(self):
        Observer.__init__(self)
        self.government = Government()
        self.population = Population()
        self.processing = []
        self.infra = []


    def tick(self, ticks):
        self.government.tick(ticks)
        self.population.tick(ticks)
        for process in self.processing:
            process.tick(ticks)

    def store(self, item):
        for struct in self.infra:
            try:
                struct.store(item)
                return
            except StorageException:
                pass
        raise StorageException("No storage space for item {1} in {0}", self, item)

    def stock(self, product):
        total = 0
        for structure in self.infra:
            total += structure.stock(product)
        return total

    def retrieve(self, product, amount):
        for struct in self.infra:
            try:
                return struct.retrieve(product, amount)
            except StorageException:
                pass
        raise StorageException("Item {1} not stored in {0}", self, item)

    def hire(self, occupation, amount):
        for worker in self.population.workers:
            if worker.occupation == occupation:
                # TODO: check and take money
                return worker.hire(amount) 
        raise EmploymentException("No {1} workers available in {0}", self, profession)

    def rent(self, building, amount):
        for struct in self.infra:
            if isinstance(struct, building):
                # TODO: check and take money
                return struct.rent(amount)
        raise OccupancyException("No {1} buildings available in {0}", self, building)

    def order(self, recipe, amount):
        # check availability first to prevent partial claims
        materials = recipe.materials
        for material in materials:
            if self.stock(material)<materials[material] * amount:
                raise ProductionException("Not enough {0} for {1}", material, recipe)
        
        process = Process(recipe, amount)  # TODO split over bulk/repeat based on availability of space/workers

        # availability checked, claim
        self.hire(recipe.professional, amount)
        self.rent(recipe.facilities, amount)
        for material in materials:
            process.materials.append(self.retrieve(material,materials[material] * amount))
        self.processing.append(process)
        process.register(self)

    def notify(self, observable, event):
        if isinstance(observable, Process) and event=='done':
            for item in observable.product:
                self.store(item)
            observable.unregister(self)
            self.processing.remove(observable)

class City(Habitation):
    pass

class Base(Habitation):
    pass

class Station(Habitation):
    pass
