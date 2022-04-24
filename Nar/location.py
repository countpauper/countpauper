from creature import Creature

class Location(object):
	def __init__(self):
		self.name = None
		self.locations = []
		self.creatures = []
		self.travel = dict()

	def __str__(self):
		return self.name

	def populate(self, name, data):
		self.name = name
		for location, sub in data.get('locations', dict()).items():
			self.locations.append(Location())
			self.locations[-1].populate(location, sub or dict())

		for travel in data.get('travel', []):
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
		for creature in data.get('creatures', []):
			self.creatures.append(Creature())
			self.creatures[-1].name = creature

	def find_creatures(self, condition=None):
		return [c for c in self. creatures if condition is None or condition(c)] + [c for l in self.locations for c in l.find_creatures(condition)]
