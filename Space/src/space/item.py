from specification import Product

class ItemException(Exception):
    pass

class Item(object):
    def __init__(self, **kwargs):
        self.specification = kwargs['specification']

    def volume(self):
        return self.amount * self.specification.volume

class StackException(ItemException):
    pass

class Unique(object):
    def __init__(self,**kwargs):
        self.amount = 1

class Stackable(object):
    def __init__(self,**kwargs):
        self.amount = kwargs['amount']

    def split(self, amount):
        if self.amount < amount:
            raise StackException("Insufficient amount", self, amount)
        self.amount -= amount
        return self.specification.create(amount)

    def stack(self, item):
        if self.specification != item.specification:
            raise StackException("Incompatible types", self, item)
        self.amount += item.amount
        item.amount = 0

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

import unittest

class Test_Stackable(unittest.TestCase):
    def test_stack(self):
        amounts = (1,2)
        product = Product(type=Food)
        some_food = product.create(amounts[0])
        more_food = product.create(amounts[1])
        more_food.stack(some_food)
        self.assertEqual(some_food.amount, 0)
        self.assertEqual(more_food.amount, amounts[0]+amounts[1])

    def test_split(self):
        amounts = (2,1)
        product = Product(type=Food)
        food = product.create(amounts[0])
        soom_food = food.split(amounts[1])
        self.assertEqual(soom_food.amount, amounts[1])
        self.assertEqual(food.amount, amounts[0]-amounts[1])

    def test_stack_mixed_spec(self):
        product = ( Product(type=Food, quality=0), Product(type=Food, quality=1))
        bad_food = product[0].create()
        good_food = product[1].create()
        self.assertRaises(StackException, lambda: bad_food.stack(good_food)) 

    def test_split_deficiency(self):
        product = Product(type=Food)
        amounts = (1,2)
        some_food = product.create(amounts[0])
        self.assertRaises(StackException, lambda: some_food.split(amounts[1]))

if __name__ == '__main__':
    unittest.main()
