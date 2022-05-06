class Item(object):
	def __init__(self, amount=0):
		self.stack=amount
		self.location = None
		self.contents = None

	def __str__(self):
		return self.name

	def description(self):
		if self.contents:
			return f"{self.name} with {self.contents.description()}"
		elif self.stack==1:
			return self.name
		else:
			return f"{self.stack}x{self.name}"

	def reduce(self, amount):
		if amount>self.stack:
			raise Exception(f"Not enough {self.name} {amount}/{self.stack}")
		self.stack -= amount
		if self.gone() and self.location:
			self.location.items.items.remove(self)
			self.location = None

	def increase(self, amount):
		self.stack += amount

	def gone(self):
		return self.stack <= 0


