class Item(object):
	def __init__(self):
		self.stack=0
		self.location = None

	def __str__(self):
		return self.name

	def reduce(self, amount):
		if amount>self.stack:
			raise Exception(f"Not enough {self.name} {amount}/{self.stack}")
		self.stack -= amount
		if self.gone() and self.location:
			self.location.items.items.remove(self)
			self.location = None

	def increase(self, amount):
		self.stack += amount

	def gone(self):
		return self.stack <= 0


# TODO: define item types in a yaml hierarchy with the roots have Item as base and the leaves their parent as base
Food = type('Item', (Item,), dict())
Bread = type('Food', (Food,), dict(weight=1, name='Bread'))
items_types=dict(bread=Bread)
