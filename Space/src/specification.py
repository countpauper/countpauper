class Specification(object):
    def __init__(self):
        pass

    def create(self, **kwargs): # TODO: terrible object properties
        if not 'amount' in kwargs: # TODO: terrible object default
            kwargs['amount'] = 1
        return self.type(specification = self, **kwargs)

    def __call__(self,**kwargs):
        return self.create(**kwargs)

class Product(Specification):
    def __init__(self, type, name='unknown', storage=None, volume=1, quality=0):
        self.type = type
        self.name = name
        self.storage = storage
        self.volume = 1
        self.quality = 0

    def __repr__(self):
        return self.name

class Building(Specification):
    def __init__(self, type, name='unknown',  space=0):
        self.type = type
        self.name = name
        self.space = space

    def __repr__(self):
        return self.name

class Profession(Specification):
    def __init__(self, name='unknown', home=None):
        self.name = name
        self.home = home

    def __repr__(self):
        return self.name

class Recipe(Specification):
    def __init__(self, product={}, materials={}, power=0, duration=0, facilities=None, professional=None):
        self.product = product
        self.materials = materials
        self.power = power
        self.duration = duration
        self.facilities = facilities
        self.professional = professional    # TODO professionals {} ?

    def __repr__(self):
        return "{} -> {}".format(self.materials, self.product)
