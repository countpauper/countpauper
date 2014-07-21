from space.time import Period
import space.constants as constants
from specification import Resource, ResourceException

class ItemException(Exception):
    pass

# private abstract base types
class _Item(Resource):
    def __init__(self, specification, amount):
        super(_Item,self).__init__(specification, amount)

    def volume(self):
        return self.amount * self.specification.volume

    def consume(self, _):
        raise ItemException("Can't consume {}", self)

class _Unique(object):
    def __init__(self,**kwargs):
        self.amount = 1

class _Stackable(object):
    def __init__(self, **kwargs):
        pass

class _Consumable(object):
    def __init__(self, **kwargs):
        pass

    def consume(self, _):
        self.amount = 0

# major category Goods
class Good(_Item, _Stackable):
    def __init__(self, **kwargs):
        _Item.__init__(self, **kwargs)
        _Stackable.__init__(self, **kwargs)

# category base classes
class _Supply(Good):
    def __init__(self, **kwargs):
        super(_Supply, self).__init__(**kwargs)

class _Material(Good):
    def __init__(self, **kwargs):
        super(_Material, self).__init__(**kwargs)

# Public types, referenced from specification
class Food(_Supply, _Consumable):
    def __init__(self, **kwargs):
        _Supply.__init__(self,**kwargs)
        _Consumable.__init__(self, **kwargs)

    def consume(self, user):
        user.hunger -= constants.calories_per_meal
        _Consumable.consume(self, user)

class Grain(_Material):
    def __init__(self, **kwargs):
        _Material.__init__(self, **kwargs)

class Water(_Material):
    def __init__(self, **kwargs):
        _Material.__init__(self, **kwargs)
