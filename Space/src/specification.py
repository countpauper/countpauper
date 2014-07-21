all = list()

class Product(object):
    pass

class Specification(object):
    def __init__(self, type, name):
        self.type = type
        self.name = name
        global all
        all.append(self)

    def create(self, amount=1, **kwargs):
        return self.type(specification=self, amount =amount, **kwargs)

    def __call__(self,**kwargs):
        return self.create(**kwargs)

    def __repr__(self):
        return self.name

class Product(Specification):
    def __init__(self, type, name='unknown', storage=None, volume=1, quality=0):
        super(Product, self).__init__(type, name)
        self.storage = storage
        self.volume = volume
        self.quality = quality

class Structure(Specification):
    def __init__(self, type, name='unknown',  space=0):
        super(Structure, self).__init__(type, name)
        self.space = space


class Service(Specification):
    def __init__(self, type, name='unknown'):
        super(Service, self).__init__(type, name)

class Profession(Specification):
    def __init__(self, type, name='unknown', home=None):
        super(Profession, self).__init__(type, name)
        self.home = home

class Recipe(Specification):
    def __init__(self, product={}, materials={}, power=0, duration=0, facilities=None, professional=None):
        super(Recipe, self).__init__(None, "recipe")    # TODO: recipe type = Process? never instanced, name? 
        self.product = product
        self.materials = materials
        self.power = power
        self.duration = duration
        self.facilities = facilities
        self.professional = professional    # TODO professionals {} ?

    def __repr__(self):
        return "{} -> {}".format(self.materials, self.product)


class ResourceException(Exception):
    pass

class StackException(ResourceException):
    pass

class Resource(object):
    def __init__(self, specification, amount):
        self.specification = specification
        self.amount = amount

    def __repr__(self):
        return "{} x {}".format(self.amount, self.specification)

    def split(self, amount):
        if self.amount < amount:
            raise StackException("Insufficient amount", self, amount)
        self.amount -= amount
        return self.specification.create(amount=amount)

    def stack(self, other):
        if self.specification != other.specification:
            raise StackException("Incompatible types", self, other)
        self.amount += other.amount
        other.amount = 0

def specify(type, **kwargs):
    for specification in all:
        if specification.type==type:
            for attr, value in kwargs.iteritems():
                if getattr(specification, attr) == value:
                    break
            else:
                return specification
    return None  # TODO raise? 
    