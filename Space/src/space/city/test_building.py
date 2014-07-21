import unittest
from specification import Structure, Product
from space.city.structure import Storage, StorageException
from space.item import Food

class Test_Container(unittest.TestCase):
    def setUp(self):
        pass

    def test_store(self):
        structure = Structure(type=Storage, space=1)
        storage = structure.create(location='None')
        product = Product(type=Food,storage=Storage)
        food = product.create()
        storage.store(food)
        self.assertEqual(storage.stock(product), 1)
        self.assertEqual(storage.space(), 0)
        self.assertEqual(food.amount, 0)

    def test_store_full(self):
        structure = Structure(type=Storage, space=1)
        storage = structure.create(location='None')
        product = Product(type=Food,storage=Storage)
        food = product.create(amount=2)
        self.assertRaises(StorageException, lambda: storage.store(food))

    def test_retrieve(self):
        structure = Structure(type=Storage, space=1)
        storage = structure.create(location='None')
        product = Product(type=Food,storage=Storage)
        food = product.create()
        storage.store(food)
        stored_food = storage.retrieve(product,1)
        self.assertEqual(stored_food.amount, 1)
        self.assertEqual(food.amount, 0)
        self.assertEqual(storage.stock(product), 0)

    def test_retrieve_deficiency(self):
        structure = Structure(type=Storage, space=1)
        storage = structure.create(location='None')
        product = Product(type=Food,storage=Storage)
        food = product.create()
        storage.store(food)
        self.assertRaises(StorageException, lambda: storage.retrieve(product,2))

    def test_storage_amount(self):
        products = 2
        spaces = 3
        structure = Structure(type=Storage, space=1)
        storage = structure.create(amount=spaces, location='None')
        product = Product(type=Food,storage=Storage)
        food = product.create(amount=products)
        storage.store(food)
        self.assertEqual(storage.stock(product),products)
        self.assertEqual(storage.space(), spaces-products)
   
    def test_storage_twice(self):
        products = 2
        spaces = 3
        structure = Structure(type=Storage, space=1)
        storage = structure.create(amount=spaces, location='None')
        product = Product(type=Food,storage=Storage)
        for p in xrange(products):
            food = product.create()
            storage.store(food)
            self.assertEqual(food.amount, 0)
        self.assertEqual(storage.stock(product),products)
        self.assertEqual(storage.space(), spaces-products)
   
    def test_retrieve_mixed_spec(self):
        structure = Structure(type=Storage, space=1)
        storage = structure.create(location='None')
        good_product = Product(type=Food,storage=Storage,quality=1)
        bad_product = Product(type=Food,storage=Storage,quality=0)
        bad_food = bad_product.create()
        storage.store(bad_food)
        self.assertRaises(StorageException, lambda: storage.retrieve(good_product,1))

if __name__ == '__main__':
    unittest.main()
