from utility.observer import Observation
class ProductionException(Exception):
    pass

class Process(Observation):
    def __init__(self, recipe, bulk=1, repeat=1):
        Observation.__init__(self)
        self.recipe = recipe
        self.timer = 0
        self.repeat = 0
        self.bulk = 0
        self.materials = []
        self.product = []

    def tick(self, ticks):
        self.timer += ticks
        print "Process {}/{}".format(self.timer, self.recipe.duration)
        if self.timer>=self.recipe.duration:    # TODO: repeat
            self.done()

    def done(self):
        print "Process done"
        products = self.recipe.product
        for product in products:
            item = product.create(amount=products[product])
            self.product.append(item)
        self.notify('done')