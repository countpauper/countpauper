import space

class Definition(object):
    def __init__(self, **kwargs):
        for kw in kwargs:
            setattr(self,kw,kwargs[kw])

class Profession(Definition):
    pass

class Building(Definition):
    pass

class Product(Definition):
    pass

class Process(Definition):
    pass
