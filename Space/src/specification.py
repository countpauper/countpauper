class Specification(object):
    def __init__(self, **kwargs):
        for prop in self.__class__.default:
            if not prop in kwargs:
                kwargs[prop] = self.__class__.default[prop]
        for kw in kwargs:
            setattr(self,kw,kwargs[kw])

    def create(self, **kwargs):
        if not 'amount' in kwargs: # TODO: terrible object default
            kwargs['amount'] = 1
        return self.type(specification = self, **kwargs)

    def __call__(self,**kwargs):
        return self.create(**kwargs)

class Product(Specification):
    default = {
                'name' : 'unknown',
                'amount' : 1,
                'storage' : None,
                'volume' : 1,
                'quality' : 0,
    }
    pass

class Building(Specification):
    default = {
               'name' : 'unknown',
               'amount' : 1,
               'space' : 0,
               }

class Profession(Specification):
    default = {
               'name' : 'unknown',
               'home' : None,
               }


class Process(Specification):
    default = {
               'power' : 0,
               'duration' : 0,
               'workplace' : None
               }


