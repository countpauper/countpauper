from creature import Creature
from item import items_types
from container import Container

class Location(object):
	def __init__(self):
		super().__init__()
		self.name = None
		self.parent = None
		self.locations = []
		self.creatures = []
		self.items = Container()
		self.travel = dict()

	def __str__(self):
		return self.name

	def locate(self, location, ignore=[]):
		if self in ignore:
			return None
		if self.name.lower() == location.lower():
			return self
		children = [l.locate(location, ignore+[self]) for l in self.locations]
		children = [c for c in children if c is not None]
		if children:
			return children[0]
		elif self.parent:
			return self.parent.locate(location, ignore=ignore+[self])
		else:
			return None

	def find(self, item_name):
		return self.items.find(item_name)

	def populate(self, name, data):
		self.name = name
		self.description = data.get('description', None)

		self.populate_rooms(data.get('locations', dict()))
		self.populate_destinations(data.get('travel', list()))
		self.populate_creatures(data.get('creatures', list()))
		self.populate_items(data.get('items',dict()))

	def populate_rooms(self, rooms):
		for location, sub in rooms.items():
			self.locations.append(Location())
			self.locations[-1].populate(location, sub or dict())
			self.locations[-1].parent = self

	def populate_destinations(self, destinations):
		for travel in destinations:
			from_location = travel.get('from')
			froms = [l for l in self.locations if l.name.lower()==from_location.lower()]
			if not froms:
				raise ValueError(f"Unknown travel origin {from_location}")
			to_location=travel.get("to")
			tos = [l for l in self.locations if l.name.lower()==to_location.lower()]
			if not tos:
				raise ValueError(f"Unknown travel destination {travel.to}")
			travel_distance=travel.get('distance',0)
			from_location=froms[0]
			to_location=tos[0]
			from_location.travel[to_location] = travel_distance
			to_location.travel[from_location] = travel_distance

	def populate_creatures(self, creatures):
		for creature_name in creatures:
			self.creatures.append(Creature())
			creature = self.creatures[-1]

			creature.name = creature_name
			creature.location = self

	def obtain(self, amount, item):
		return self.items.add(self, amount, item)

	def populate_items(self, items):
		for item_name, stack in items.items():
			item_type=items_types.get(item_name.lower())
			if item_type:
				new_item = item_type()
				new_item.stack = stack
				self.obtain(stack, new_item)
			else:
				raise ValueError(f"Unknown item {item_name} in {+self.name}")

	def blurb(self):
		return f"{self.name}\n{self.description}\nContaining {self.items.description()}\n{', '.join(c.name for c in self.creatures)}"

	def find_creatures(self, condition=None):
		return [c for c in self. creatures if condition is None or condition(c)] + [c for l in self.locations for c in l.find_creatures(condition)]
