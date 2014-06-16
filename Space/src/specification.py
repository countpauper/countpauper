class Specification(object):
    def __init__(self, **kwargs):
        for kw in kwargs:
            setattr(self,kw,kwargs[kw])

    def create(self, amount=1):
        return self.type(specification = self, amount = amount)


class Product(Specification):
    pass

class Building(Specification):
    pass

class Profession(Specification):
    pass

class Process(Specification):
    pass
