
class Contents:
	def __init__(self, owner=None, data=dict()):
		self.items = []
		self.populate(owner, data)

	def find(self, item):
		matches = [i for i in self.items if i.name.lower() == item.lower()]
		if not matches:
			return None
		return matches[0]

	def add(self, owner, item, amount=None):
		if amount is None or amount==item.stack:
			if item.location:
				item.location.items.remove(item)
			item.location = owner
			self.items.append(item)
		else:
			item.reduce(amount)
			owned_stack=self.find(item.name)
			if not owned_stack:
				owned_stack=item.__class__()
				owned_stack.location = owner
				self.items.append(owned_stack)
			owned_stack.increase(amount)

	def populate(self, owner, data):
		from items import items_types
		# list of items, 1 each
		if type(data) == list:
			data={i : 1 for i in data}

		for item_name, properties in data.items():
			item_type = items_types.get(item_name.lower())
			if item_type:
				if type(properties) == dict:
					properties['amount']=properties.get('amount',1)
					new_item = item_type(**properties)
				else:
					new_item = item_type(properties)
				self.add(owner, new_item)
			else:
				raise ValueError(f"Unknown item {item_name} in {+self.name}")

	def description(self):
		return ', '.join(i.description() for i in self.items) or "Nothing"