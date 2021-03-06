import unittest
from item import Food, StackException
from specification import Product

class Test_Stackable(unittest.TestCase):
    def test_stack(self):
        amounts = (1,2)
        product = Product(type=Food)
        some_food = product.create(amount=amounts[0])
        more_food = product.create(amount=amounts[1])
        more_food.stack(some_food)
        self.assertEqual(some_food.amount, 0)
        self.assertEqual(more_food.amount, amounts[0]+amounts[1])

    def test_split(self):
        amounts = (2,1)
        product = Product(type=Food)
        food = product.create(amount=amounts[0])
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
        some_food = product.create(amount=amounts[0])
        self.assertRaises(StackException, lambda: some_food.split(amounts[1]))

if __name__ == '__main__':
    unittest.main()
