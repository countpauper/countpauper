from utility.observer import Observation
from space.time import Period

class ProductionException(Exception):
    pass

class Process(Observation):
    def __init__(self, recipe, bulk=1, repeat=1):
        Observation.__init__(self)
        self.recipe = recipe
        self.repeat = repeat
        self.bulk = bulk
        self.timer = Period()
        self.materials = None
        self.product = []

    def __repr__(self):
        return "{} x {} x {}".format(self.repeat, self.bulk, self. recipe)

    def tick(self, time, period):
        if self.complete(): # TODO: proper state machine
            return 
        if not self.job.ready():
            return
        else:
            self.job.extend(period)

        if not self.materials.ready():
            return

        self.timer += period
        if self.timer>=self.recipe.duration:    # TODO: repeat
            self.done()
        # TODO: pay wages and rent here instead of in front

    def complete(self):
        return self.product

    def total_price(self):
        return self.materials.total_price() + self.job.total_price()

    def done(self):
        products = self.recipe.product
        for product in products:
            item = product.create(amount=products[product] * self.bulk)
            self.product.append(item)
        self.job.finalize()
        for item in [offer.resource for offer in self.materials]:
            item.consume()
        self.notify('done')