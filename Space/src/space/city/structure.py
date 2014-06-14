class StorageException(Exception):
    pass

class Structure(object):
    def __init__(self, location, type, total = 0):
        self.location = location
        self.type = type
        self.store = []
        self.total = total
        self.occupancy = 0

    def store(self, item):
        if item.type.store != self.type:
            raise StorageException("Wrong storage type {0} for {1}", self, item)
        if item.volume() > self.space():
            raise StorageException("No room in {0} for {1}", self, item)
        self.item.append(item)
        # TODO: stack 

    def stock(self, product):
        total = 0
        for item in self.store:
            if item.type == product:
                total += item.amount
        return total

    def retrieve(self,product, amount):
        for item in self.store:
           if item.type == product:
            # TODO unstack 
            return item
        raise StorageException("Item {1} not stored in {0}", self, item)

    def space(self):
        space = self.type.space * self.total
        for item in store:
            space -= item.volume()
        return space
