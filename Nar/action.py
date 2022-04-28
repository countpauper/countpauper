class Action(object):
	def __init__(self):
		self.progress=None


class Go(Action):
	def __init__(self, destination):
		super().__init__()
		self.destination = destination

	def execute(self, actor, world):
		destination = actor.location.locate(self.destination)
		if destination:
			# for general areas, go to the first specific leaf location as default entrance
			while destination.locations:
				destination = destination.locations[0]
			actor.move(destination)
		else:
			raise Exception("Failed to find destination "+self.destination)

class Get(Action):
	def __init__(self, item):
		super().__init__()
		self.item = item

	def execute(self, actor, world):
		item = actor.location.find(self.item)
		if item:
			actor.obtain(1, item)
		else:
			raise Exception("Failed to find item "+self.destination)

# Go = type('Action', (Action,), dict())

actions = dict(go=Go, get=Get)

