# exceptions

class ItemException(Exception):
    pass

class StackException(ItemException):
    pass

# private abstract base types
class _Item(object):
    def __init__(self, **kwargs):
        self.specification = kwargs['specification']

    def volume(self):
        return self.amount * self.specification.volume


class _Unique(object):
    def __init__(self,**kwargs):
        self.amount = 1

class _Stackable(object):
    def __init__(self,**kwargs):
        self.amount = kwargs['amount']

    def split(self, amount):
        if self.amount < amount:
            raise StackException("Insufficient amount", self, amount)
        self.amount -= amount
        return self.specification.create(amount=amount)

    def stack(self, item):
        if self.specification != item.specification:
            raise StackException("Incompatible types", self, item)
        self.amount += item.amount
        item.amount = 0

class _Consumable(object):
    def __init__(self, **kwargs):
        pass

# category base classes
class _Supply(_Item, _Stackable):
    def __init__(self, **kwargs):
        _Item.__init__(self, **kwargs)
        _Stackable.__init__(self, **kwargs)

class _Material(_Item, _Stackable):
    def __init__(self, **kwargs):
        _Item.__init__(self, **kwargs)
        _Stackable.__init__(self, **kwargs)

# Public types, referenced from specification
class Food(_Supply, _Consumable):
    def __init__(self, **kwargs):
        _Supply.__init__(self,**kwargs)
        _Consumable.__init__(self, **kwargs)

class Grain(_Material):
    def __init__(self, **kwargs):
        _Material.__init__(self, **kwargs)

class Water(_Material):
    def __init__(self, **kwargs):
        _Material.__init__(self, **kwargs)
