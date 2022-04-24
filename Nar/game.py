from world import World
from ai import AI

class Game(object):
	def __init__(self):
		self.players = [AI()]
		self.world = World()

