class Action(object):
	def __init__(self):
		self.progress=None


class Go(Action):
	def __init__(self, destination):
		super().__init__()
		self.destination = destination

	def execute(self, actor, world):
		route = actor.location.travel(self.destination)
		if route:
			# for general areas, go to the first specific leaf location as default entrance
			destination = route[-1]
			while destination.locations:
				destination = destination.locations[0]
			actor.move(destination)
		else:
			raise Exception("Failed to find a route to "+self.destination)

def parse_items(args):
	if len(args) >= 2:
		return args[1], int(args[0])
	elif args:
		return args[0], 1
	else:
		raise ValueError("No items specified")

class Get(Action):
	def __init__(self, *args):
		super().__init__()
		self.item, self.amount = parse_items(args)

	def execute(self, actor, world):
		item = actor.location.find(self.item)
		if item:
			actor.obtain(item, self.amount)
		else:
			raise Exception(f"Failed to find {self.item} in {actor.location.name}")


class Drop(Action):
	def __init__(self, *args):
		super().__init__()
		self.item, self.amount = parse_items(args)

	def execute(self, actor, world):
		item = actor.find(self.item)
		if item:
			actor.location.place(item, self.amount)
		else:
			raise Exception(f"Failed to find {self.item} in {actor.name}")


# Go = type('Action', (Action,), dict())

actions = dict(go=Go, get=Get, drop=Drop)

