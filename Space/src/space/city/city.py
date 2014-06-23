from agent import Agent, EmploymentException
from structure import StorageException
from utility.observer import Observer
from utility.table import Table
from market import GoodsMarket

class Habitation(Observer):
    def __init__(self, name):
        Observer.__init__(self)
        self.name = name
        self.actors = []
        self.infra = []
        self.inventory = Table(["time","bread","wheat","H2O"])
        self.inventory.open('inventory.csv')

    def __repr__(self):
        return self.name

    def tick(self, time, period):
        for actor in self.actors:
            actor.tick(self, time, period)
        self._write_inventory(time)

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
        # TODO isinstance check requires public Agent, duck type?
        # This may not be needed if workers are found through market at all 
        return [worker for worker in self.actors if isinstance(worker, Agent) and worker.occupation == occupation]

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

class City(Habitation):
    pass

class Base(Habitation):
    pass

class Station(Habitation):
    pass
