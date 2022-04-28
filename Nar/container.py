class Container:
	def __init__(self):
		self.items = []

	def find(self, item):
		matches = [i for i in self.items if i.name.lower() == item.lower()]
		if not matches:
			return None
		return matches[0]

	def add(self, owner, amount, item):
		item.reduce(amount)
		owned_stack=self.find(item.name)
		if not owned_stack:
			owned_stack=item.__class__()
			owned_stack.location = owner
			self.items.append(owned_stack)
		owned_stack.increase(amount)

	def description(self):
		return ', '.join(i.name if i.stack==1 else f"{i.stack}x{i.name}" for i in self.items) or "Nothing"