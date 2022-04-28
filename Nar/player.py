from random import choice
from action import actions

class Player(object):
	def __init__(self):
		self.name = 'Player #1'
		self.controlled=[]

	def __str__(self):
		return self.name

	def parse(command):
		action_command, args=command.strip(" ").split(" ",maxsplit=1)
		args=args.strip(" ").split(',')
		action = actions.get(action_command)
		if action:
			return action(args)
		else:
			return None

	def control(self, world):
		if not self.controlled:
			self.reincarnate(world)
		creature = self.controlled[0]

		print(f"{self.name}: {creature.description()}")
		print(f"You're at {creature.location.blurb()}\n")
	
		act = self.parse(input())
		if act:
			try:
				act.execute(creature, world)
			except Exception as e:
				print(e)

	def reincarnate(self, world):
		creatures=world.find_creatures(lambda c : c.player is None)
		if not creatures:
			raise Exception("No NPCs available to reincarnate "+self.name)
		creature = choice(creatures)
		creature.player = self
		self.controlled = [creature]

