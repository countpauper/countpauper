class Creature(object):
	def __init__(self):
		self.name = None
		self.intelligence, self.agility, self.strength = 0, 0, 0
		self.age = 0
		self.location, self.player, self. task = None, None, None
		self.conditions = dict()
		self.carried = list()
		self.relations = dict()
		self.knowledge = list()

	def __str__(self):
		return self.name


