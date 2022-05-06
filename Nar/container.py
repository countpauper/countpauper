from item import Item
from contents import Contents

class Container(Item):

	def __init__(self, items=dict(), amount=0):
		super().__init__(amount)
		self.open = not type(self).door
		self.locked = not type(self).lock
		self.key = None
		self.contents = Contents(self, items)
		# per type: openable (or something), lockable, size, loc