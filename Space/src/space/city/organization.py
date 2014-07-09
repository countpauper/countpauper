from utility.observer import Observer
from process import Process, ProductionException
from specification import Recipe

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

    def order(self, location, recipe, bulk):
        # check availability first to prevent partial claims
        materials = recipe.materials
        for material in materials:
            if location.stock(material)<materials[material] * bulk:
                raise ProductionException("Not enough {0} for {1}", material, recipe)
        
        process = Process(recipe, bulk)  # TODO split over bulk/repeat based on availability of space/workers
        process.location = location # TODO: location needed to release, instead store individual claims and release them

        for product,amount in recipe.product.iteritems():
            if sum([building.space() for building in location._buildings(product.storage)]) <= product.volume * bulk:
                raise ProductionException("Not enough storage in {0} for {1}", self, product)

        # availability checked, claim
        for product,amount in recipe.product.iteritems():
            location.reserve(product.storage, product.volume*amount*bulk)
       
        process.workers = location.hire(recipe.professional, bulk)   # TODO: fire these workers directly instead of going through city
        process.buildings = location.rent(recipe.facilities, bulk)  # TODO release this estate
        for material in materials:
            process.materials.append(location.retrieve(material,materials[material] * bulk))
        self.processing.append(process)
        process.register(self)
        return process

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


    def produce(self, recipe, location):
        # TODO: figure out demand for recipe's product & amount (money to be made, inverse quote)
        # if there is no demand, don't product (yet)
        # Else attempt to buy all ingredients, (cancel/collect outstanding offers)
        # if all are there and demand is there: produce
        # TODO estimate profit potential
        pass

class Guild(Business):
    def __init__(self, recipe):
        super(Guild,self).__init__()
        self.recipe = recipe

    def think(self, location):
        self.produce(self.recipe, location)



