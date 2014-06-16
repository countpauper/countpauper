class Specification(object):
    def __init__(self):
        pass

    def create(self, **kwargs):
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

class Building(Specification):
    def __init__(self, type, name='unknown',  space=0):
        self.type = type
        self.name = name
        self.space = space

class Profession(Specification):
    def __init__(self, name='unknown', home=None):
        self.name = name
        self.home = home


class Process(Specification):
    def __init__(self, power=0, duration=0, workplace=None):
        self.power = power
        self.duration = duration
        self.workplace = workplace

