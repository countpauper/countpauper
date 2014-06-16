from specification import Building



class StorageException(Exception):
    pass


class Structure(object):
    def __init__(self, **kwargs):
        self.location = kwargs['location']
        self.specification = kwargs['specification']
        self.amount = kwargs['amount']

    def store(self, item):
        raise StorageException("Not a container")

class Container(object):
    def __init__(self, **kwargs):
        self.contents = []

    def store(self, item):
        # TODO: check space, current container
        if not isinstance(self, item.specification.storage):
            raise StorageException("Wrong storage type {0} for {1}", self, item)
        if item.volume() > self.space():
            raise StorageException("No room in {0} for {1}", self, item)
        self.contents.append(item)
        # TODO: stack 

    def stock(self, product):
        total = 0
        for item in self.contents:
            if item.specification == product:
                total += item.amount
        return total

    def retrieve(self, product, amount):
        for item in self.contents:
           if item.specification == product:
            self.contents.remove(item)
            # TODO: unstack/split
            return item
        raise StorageException("Item {1} not stored in {0}", self, item)

    def space(self):
        space = self.specification.space * self.amount
        for item in self.contents:
            space -= item.volume()
        return space

class Storage(Container, Structure):
    def __init__(self, **kwargs):
        Container.__init__(self, **kwargs)
        Structure.__init__(self,**kwargs)


class Home(Structure):
    def __init__(self, **kwargs):
        super(Home, self).__init__(**kwargs)
        self.inhabitants = []

class Workplace(Structure):
    def __init__(self, **kwargs):
        super(Workplace, self).__init__(**kwargs)

class Baker(Workplace):
    def __init__(self, **kwargs):
        super(Baker, self).__init__(**kwargs)

class Farm(Workplace):
    def __init__(self, **kwargs):
        super(Farm, self).__init__(**kwargs)


