class Definition(object):
    def __init__(self, **kwargs):
        for kw in kwargs:
            setattr(self,kw,kwargs[kw])

class Profession(Definition):
    pass

class Building(Definition):
    pass

class House(Building):
    def __init__(self,**kwargs):
        super(House,self).__init__(**kwargs)
        type='home'

class Workplace(Building):
    pass

class Storage(Building):
    def __init__(self, **kwargs):
        super(Storage,self).__init__(**kwargs)
        type='storage'

class Product(Definition):
    pass

class Supply(Product):
    pass

class Material(Product):
    pass

class Process(Definition):
    pass
