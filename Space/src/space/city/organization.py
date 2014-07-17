from utility.observer import Observer
from process import Process, ProductionException
from specification import Recipe
from market import Batch, SupplyException

class Organization(Observer):
    def __init__(self):
        self.money = 0
        self.processing = []

    def tick(self, location, time, period):
       for process in self.processing:      
            process.tick(time, period)
       # NB: one update is wasted placing orders because either the resources aren't freed or the 
       #  new orders aren't ticked, scheduler should solve this TODO
       self.think(location)

    def notify(self, observable, event):
        if isinstance(observable, Process) and event=='done':
            process = observable
            location = process.location 
            for item in process.product:
                location.store(item)
            location = process.location
            location.fire(process.recipe.professional, process.bulk)
            location.release(process.recipe.facilities, process.bulk)
            self.processing.remove(process)
            process.unregister(self)
            process.location = None

class Government(Organization):
       
    def __init__(self, location):
        super(Government,self).__init__(location)
        self.responsibilities = []  


    def think(self, location):
        for recipe in self.responsibilities:
            # TODO: estimate need
            amounts = location.availability(recipe)
            amount = min(amounts)
            if amount:
                self.order(location, recipe, amount)

class Business(Organization):
    def __init__(self):
        super(Business,self).__init__()
        self.offers = list()

    def think(self, location):
        for process in self.processing:    
            if process.complete():
                for item in process.product:
                    try: # TODO code duplication (see Batch) batch sell?
                        price = location.quote(item.specification, 1)
                    except SupplyException:
                        price = 1000
                    self.offers.append(location.sell(self,item,price))
                self.processing.remove(process)
            elif not process.materials.ready():
                for offer in process.materials.offers:
                    offer.adjust(offer.price + 1 + offer.price/10)
        for offer in self.offers:
            if offer.ready():
                _ = offer.claim()
                self.offers.remove(offer)
            else:
                offer.adjust(offer.price - offer.price/10)

    def produce(self, recipe, location):
        process = Process(recipe)

        batch = Batch(self, recipe.materials, location)
        process.materials = batch
        self.processing.append(process)

class Guild(Business):
    def __init__(self, recipe):
        super(Guild,self).__init__()
        self.recipe = recipe

    def think(self, location):
        super(Guild, self).think(location)
        if not self.processing:
            self.produce(self.recipe, location)



