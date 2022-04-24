from random import choice

class Player(object):
	def __init__(self):
		self.name = 'Player #1'
		self.controlled=[]

	def __str__(self):
		return self.name

	def control(self, world):
		if not self.controlled:
			self.reincarnate(world)
		creature = self.controlled[0]

		print(f'{creature.name} ({self.name}) :')
		command = input()

	def reincarnate(self, world):
		creatures=world.find_creatures(lambda c : c.player is None)
		if not creatures:
			raise Exception("No NPCS to reincarnate "+self.name)
		creature = choice(creatures)
		creature.player = self
		self.controlled = [creature]

