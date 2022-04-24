class Item(object):
	def __init__(self):
		self.weight = 0
		self.name = None

	def __str__(self):
		return self.name

# TODO: define item types in a yaml hierarchy with the roots have Item as base and the leaves their parent as base
Food = type('Item', (Item,), dict(attr=100))
Bread = type('Food', (Food,), dict(weight=1))
