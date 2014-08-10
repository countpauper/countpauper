from agent import Agent, EmploymentException
from building import StorageException
from utility.observer import Observer
from utility.table import Table
from market.goods import GoodsMarket
from market.jobs import JobMarket
from market import TradeException

class Habitation(Observer):
    def __init__(self, name):
        Observer.__init__(self)
        self.name = name
        self.actors = []
        self.infra = []
        self.markets = [ GoodsMarket(), JobMarket() ]
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
    
    def _store(self, item):
        store = self._buildings(item.specification.storage)
        if not store:
            raise StorageException("No store for item {1} in {0}", self, item)
        store[0].store(item)    # TODO: spread over stores

    def sell(self, vendor, item, price):
        return self._market(item.specification).sell(vendor, item, price)

    def stock(self, product):
        total = 0
        for structure in self.infra:
            total += structure.stock(product)
        return total

    def _retrieve(self, item, amount):
        """Return unstacked item, currently stored in container structures
        item may be a product specification or the name of a product
        the result is a new item with the type of the specification of the given item, 
        but the specified amount and the same owner. The stored item's amount is reduced
        by the given amount and removed entirely from storage if amount is reduced to 0.
        If not enough of the specified amount is stored, an exception is raised"""
        for struct in self.infra:
            try:
                return struct.retrieve(item, amount)
            except StorageException:
                pass
        raise StorageException("Product {1} not stored in {0}", self, product)

    def _market(self, product):
        """Find market that carries product specification or class type"""
        for market in self.markets:
            if market.trades(product):
                return market
        raise TradeException("No market in {0} trades {1}", self, product)

    def quote(self, product, amount):
        return self._market(product). quote(product, amount)

    def turnover(self, product, amount):
        return self._market(product).turnover(product, amount)

    def shop(self, product):
        return self._market(product).shop(product)

    def buy(self, buyer, product, amount, price):
        return self._market(product).buy(buyer, product, amount, price)

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

    def _buildings(self, structure):
        return [struct for struct in self.infra if isinstance(struct,structure)]

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
