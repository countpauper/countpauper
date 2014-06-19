from population import Population, EmploymentException
from structure import StorageException
from process import Process, ProductionException
from specification import Recipe
from utility.observer import Observer
from organization import Government
from utility.table import Table

class Habitation(Observer):
    def __init__(self, name):
        Observer.__init__(self)
        self.name = name
        self.government = Government([self])
        self.population = Population()
        self.processing = []
        self.infra = []
        self.inventory = Table(["time","bread","wheat","H2O"])
        self.inventory.open('inventory.csv')

    def __repr__(self):
        return self.name

    def tick(self, space_time, period):
        self.government.tick(space_time, period)    # NB: one update is wasted manually repeating orders
        self.population.tick(space_time, period)
        for process in self.processing:
            process.tick(space_time, period)
        self._write_inventory(space_time)

    def _write_inventory(self, space_time):
        stock = dict([(item,self.stock(item)) for item in self.inventory.header])
        stock["time"] = space_time
        self.inventory.append( stock )

    def reserve(self, building, volume):
        structs = self._buildings(building)
        if not structs:
            raise OccupancyException("No {1} storage available in {0}", self, building)
        structs[0].reserve(volume)  # TODO: spread over stores
    
    def store(self, item):
        store = self._buildings(item.specification.storage)
        if not store:
            raise StorageException("No store for item {1} in {0}", self, item)
        store[0].store(item)    # TODO: spread over stores

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
        raise StorageException("Product {1} not stored in {0}", self, product)

    def _workers(self, occupation):
        return [worker for worker in self.population.workers if worker.occupation == occupation]

    def hire(self, occupation, amount):
        workers = self._workers(occupation)
        if not workers:
            raise EmploymentException("No {1} workers available in {0}", self, profession)
        workers[0].hire(amount)  # TODO: count em and hire multiple results

    def fire(self, occupation, amount):
        workers = self._workers(occupation)
        if not workers:
            raise EmploymentException("No {1} workers exist in {0}", self, profession)
        workers[0].fire(amount) # TODO: count em and fire multiple

    def _buildings(self, building):
        return [struct for struct in self.infra if isinstance(struct,building)]



    def rent(self, building, amount):
        structs = self._buildings(building)
        if not structs:
            raise OccupancyException("No {1} buildings available in {0}", self, building)
        structs[0].rent(amount) # TODO: count em and rent multiple?

    def release(self, building, amount):
        structs = self._buildings(building)
        if not structs:
            raise OccupancyException("No {1} buildings exist in {0}", self, building)
        structs[0].release(amount) # TODO: count em and weep
            
    def order(self, recipe, bulk):
        # check availability first to prevent partial claims
        materials = recipe.materials
        for material in materials:
            if self.stock(material)<materials[material] * bulk:
                raise ProductionException("Not enough {0} for {1}", material, recipe)
        
        process = Process(recipe, bulk)  # TODO split over bulk/repeat based on availability of space/workers
        
        for product,amount in recipe.product.iteritems():
            if sum([building.space() for building in self._buildings(product.storage)]) <= product.volume * bulk:
                raise ProductionException("Not enough storage in {0} for {1}", self, product)

        # availability checked, claim
        for product,amount in recipe.product.iteritems():
            self.reserve(product.storage, product.volume*amount*bulk)
       
        self.hire(recipe.professional, bulk)
        self.rent(recipe.facilities, bulk)
        for material in materials:
            process.materials.append(self.retrieve(material,materials[material] * bulk))
        self.processing.append(process)
        process.register(self)

    def availability(self, recipe):
        """Return amounts for availability of: materials, place, professional, power, storage"""
        materials = [self.stock(material) // recipe.materials[material] for material in recipe.materials]

        stores = [(sum([building.space() for building in self._buildings(product.storage)]) // amount * product.volume) 
                    for product,amount in recipe.product.iteritems()]
        workers = sum([worker.idle() for worker in self._workers(recipe.professional)])
        facility = sum([building.available() for building in self._buildings(recipe.facilities)])

        if materials:
            return min(materials), min(stores), facility, workers # TODO: power/storage
        else:
            return min(stores), facility, workers

    def notify(self, observable, event):
        if isinstance(observable, Process) and event=='done':
            process = observable
            for item in process.product:
                self.store(item)
            process.unregister(self)
            self.processing.remove(process)
            self.fire(process.recipe.professional, process.bulk)
            self.release(process.recipe.facilities, process.bulk)

class City(Habitation):
    pass

class Base(Habitation):
    pass

class Station(Habitation):
    pass
