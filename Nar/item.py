class Item(object):
	def __init__(self):
		self.stack=1
		self.location = None

	def __str__(self):
		return self.name

# TODO: define item types in a yaml hierarchy with the roots have Item as base and the leaves their parent as base
Food = type('Item', (Item,), dict())
Bread = type('Food', (Food,), dict(weight=1, name='Bread'))
items_types=dict(bread=Bread)
