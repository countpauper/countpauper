class Creature(object):
	def __init__(self):
		self.name = None
		self.intelligence, self.agility, self.strength = 0, 0, 0
		self.age = 0
		self.location, self.player, self. task = None, None, None
		self.conditions = dict()
		self.items = list()
		self.relations = dict()
		self.knowledge = list()

	def description(self):
		return f"{self.name} Carrying: {','.join(i.name for i in self.items) or 'Nothing'}"

	def move(self, destination):
		self.location.creatures.remove(self)
		self.location = destination
		self.location.creatures.append(self)

	def obtain(self, amount, item):
		# TODO: amounts and stacks
		item.location.items.remove(item)
		item.location = self
		self.items.append(item)

	def __str__(self):
		return self.name


