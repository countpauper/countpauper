from container import Container

class Creature(object):
	def __init__(self):
		self.name = None
		self.intelligence, self.agility, self.strength = 0, 0, 0
		self.age = 0
		self.location, self.player, self. task = None, None, None
		self.conditions = dict()
		self.items = Container()
		self.relations = dict()
		self.knowledge = list()

	def description(self):
		return f"{self.name} Carrying: {self.items.description()}"

	def move(self, destination):
		self.location.creatures.remove(self)
		self.location = destination
		self.location.creatures.append(self)

	def find(self, item_name):
		return self.items.find(item_name)

	def obtain(self, amount, item):
		return self.items.add(self, amount, item)

	def __str__(self):
		return self.name


