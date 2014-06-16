class Item(object):
    def __init__(self, **kwargs):
        self.specification = kwargs['specification']

    def volume(self):
        return self.amount * self.specification.volume

class Stackable(object):
    def __init__(self,**kwargs):
        self.amount = kwargs['amount']

    def split(self, amount):
        return # TODO return remaining

    def stack(self, item):
        pass # TODO add amount, (check type)

class Consumable(object):
    def __init__(self, **kwargs):
        pass

class Supply(Item, Stackable):
    def __init__(self, **kwargs):
        super(Supply, self).__init__(**kwargs)
        Stackable.__init__(self, **kwargs)

class Food(Supply, Consumable):
    def __init__(self, **kwargs):
        super(Food, self).__init__(**kwargs)
        Consumable.__init__(self, **kwargs)

class Material(Item, Stackable):
    def __init__(self, **kwargs):
        super(Material, self).__init__(**kwargs)
        Stackable.__init__(self, **kwargs)

class Grain(Material):
    def __init__(self, **kwargs):
        super(Grain, self).__init__(**kwargs)
