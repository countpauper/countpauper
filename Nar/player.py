from random import choice
from action import actions

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

		print(f"{creature.name} ({self.name}) : You're at {creature.location.blurb()}\n")
		command = input()
		action_command, args=command.split(" ",maxsplit=1)
		args=args.split(',go')
		action = actions.get(action_command)
		if action:
			act = action(*args)
			act.execute(self.controlled[0], world)


	def reincarnate(self, world):
		creatures=world.find_creatures(lambda c : c.player is None)
		if not creatures:
			raise Exception("No NPCs available to reincarnate "+self.name)
		creature = choice(creatures)
		creature.player = self
		self.controlled = [creature]

