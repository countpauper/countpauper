from creature import Creature
from contents import Contents

class Location(object):
	def __init__(self, name=None, parent=None, data=dict()):
		super().__init__()
		self.name = name
		self.parent = parent
		self.locations = []
		self.creatures = []
		self.items = Contents()
		self.path = dict()
		self.populate(data)

	def __str__(self):
		return self.name

	def locate(self, location, closed=[]):
		if self in closed:
			return None
		if self.name.lower() == location.lower():
			return self
		children = [l.locate(location, closed+[self]) for l in self.locations]
		children = [c for c in children if c is not None]
		if children:
			return children[0]
		elif self.parent:
			return self.parent.locate(location, closed=closed+[self])
		else:
			return None

	def travel(self, destination, closed=list()):
		if self in closed:
			return None
		destinations = [d for d in self.path if destination.lower() in d.name.lower()]
		if destinations:
			return [destinations[0]]
		else:
			return None # First only one depth
			# TODO then depth first search, add self to closed list, return a route with intermediate destinations and lengths
			# later keep searching for shortest paths A* requires an estimate of proximity to destination. This may be hard to do without coordinates
			# dijkstra algorithm (take node distance into account at least)

	def find(self, item_name):
		return self.items.find(item_name)

	def populate(self, data):
		self.details = data.get('description', None)
		self.populate_rooms(data.get('locations', dict()))
		self.populate_destinations(data.get('travel', list()))
		self.populate_creatures(data.get('creatures', dict()))
		self.items.populate(self, data.get('items',dict()))

	def populate_rooms(self, rooms):
		for name, sub in rooms.items():
			self.locations.append(Location(name, self, sub))

	def populate_destinations(self, destinations):
		if not destinations:
			return
		for travel in destinations:
			from_location = self.locate(travel.get('from'))
			if not from_location:
				raise ValueError(f"Unknown travel origin {from_location}")
			to_location=self.locate(travel.get("to"))
			if not to_location:
				raise ValueError(f"Unknown travel destination {travel.to}")
			travel_distance=travel.get('distance',0)
			from_location.path[to_location] = travel_distance
			to_location.path[from_location] = travel_distance

	def populate_creatures(self, creatures):
		if type(creatures) == list:
			for creature_name in creatures:
				self.creatures.append(Creature(creature_name, self, dict()))
		else:
			for creature_name, data in creatures.items():
				self.creatures.append(Creature(creature_name, self, data))

	def place(self, item, amount=None):
		return self.items.add(self, item, amount)

	def description(self):
		return f"""{self.name}
{self.details}
Containing {self.items.description()}
{', '.join(c.name for c in self.creatures)}
Exits to {", ".join(l.name for l in self.path)}"""

	def find_creatures(self, condition=None):
		return [c for c in self. creatures if condition is None or condition(c)] + [c for l in self.locations for c in l.find_creatures(condition)]
