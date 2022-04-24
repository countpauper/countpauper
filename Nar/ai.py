class AI(object):
	def __init__(self):
		self.programs = []
		self.controlled = []

	def control(self, world):
		self.control = world.find_creatures(lambda c: c.player is None)
