from utility.observer import Observer
from process import Process, ProductionException
from specification import Recipe
from market import Batch, SupplyException
from math import ceil

PROFIT_MARGIN = 2.0
PRICE_DROP = 0.9
PRICE_INCREASE = 1.1

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
                    price = ceil(process.total_price() * PROFIT_MARGIN)
                    self.offers.append(location.sell(self,item,price))
                self.processing.remove(process)
            elif not process.materials.ready():
                for offer in process.materials.offers:
                    offer.adjust(ceil(offer.price * PRICE_INCREASE))
            if not process.job.ready():
                process.job.adjust(ceil(process.job.price * PRICE_INCREASE))
        # TODO: adjust price by looking at market instead of blindly increasing
        for offer in self.offers:
            if offer.ready():
                _ = offer.claim()
                self.offers.remove(offer)
            else:
                offer.adjust(ceil(offer.price*PRICE_DROP))

    def produce(self, recipe, location):
        process = Process(recipe)
        # todo: move to process
        batch = Batch(self, recipe.materials, location)
        process.materials = batch
        # TODO: round down duration to days or weeks
        process.job = location.buy(self, recipe.professional, recipe.duration,10)
        self.processing.append(process)
       
class Guild(Business):
    def __init__(self, recipe):
        super(Guild,self).__init__()
        self.recipe = recipe

    def think(self, location):
        super(Guild, self).think(location)
        if not self.processing:
            self.produce(self.recipe, location)



