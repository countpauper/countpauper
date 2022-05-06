from contents import Contents

class Creature(object):
	def __init__(self, name=None, location=None, data=None):
		self.name = name
		self.intelligence, self.agility, self.strength = 0, 0, 0
		self.age = 0
		self.location = location
		self.player, self. task = None, None
		self.conditions = dict()
		self.items = Contents()
		self.relations = dict()
		self.knowledge = list()
		self.populate(data or dict())

	def description(self):
		return f"{self.name} Carrying: {self.items.description()}"

	def move(self, destination):
		self.location.creatures.remove(self)
		self.location = destination
		self.location.creatures.append(self)

	def find(self, item_name):
		return self.items.find(item_name)

	def obtain(self, item, amount=None):
		return self.items.add(self, item, amount)

	def populate(self, data):
		self.intelligence = data.get('intelligence',0)
		self.agility = data.get('agility',0)
		self.strength = data.get('strength',0)
		self.age = data.get('age',0)
		self.items.populate(self, data.get('items', dict()))

	def __str__(self):
		return self.name


