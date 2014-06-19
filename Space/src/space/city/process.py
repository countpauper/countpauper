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
        self.materials = []
        self.product = []

    def tick(self, time, period):
        self.timer += period
        print "Process {} {} {}/{}".format(self.bulk, self.recipe.product, self.timer, self.recipe.duration)
        if self.timer>=self.recipe.duration:    # TODO: repeat
            self.done()

    def done(self):
        print "Process done"
        products = self.recipe.product
        for product in products:
            item = product.create(amount=products[product])
            self.product.append(item)
        self.notify('done')