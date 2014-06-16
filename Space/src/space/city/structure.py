class StorageException(Exception):
    pass


# private abstract base classes

class _Structure(object):
    def __init__(self, **kwargs):
        self.location = kwargs['location']
        self.specification = kwargs['specification']
        self.amount = kwargs['amount']

    def store(self, item):
        raise StorageException("Not a container")

class _Container(object):
    def __init__(self, **kwargs):
        self.contents = []

    def store(self, item):
        # TODO: check space, current container
        if not isinstance(self, item.specification.storage):
            raise StorageException("Wrong storage type {0} for {1}", self, item)
        if item.volume() > self.space():
            raise StorageException("No room in {0} for {1}", self, item)
        same_item = self.find(item.specification)
        if same_item:
            same_item[0].stack(item)
        else:
            same_item = item.specification.create(amount=0)
            self.contents.append(same_item)
            same_item.stack(item)

    def find(self, product):
        return [item for item in self.contents if item.specification==product]

    def stock(self, product):
        return sum([item.amount for item in self.find(product)])

    def retrieve(self, product, amount):
        items = self.find(product)
        for item in items:
            if item.amount < amount:
                raise StorageException("Insuffient amount [{1}] of {0} stored", item, amount)
            retrieved_item = item.split(amount)
            if not item.amount:
                self.contents.remove(item) 
            return retrieved_item
        raise StorageException("Item {1} not stored in {0}", self, product)

    def space(self):
        space = self.specification.space * self.amount
        for item in self.contents:
            space -= item.volume()
        return space

class _Workplace(object):
    def __init__(self, **kwargs):
        pass

class _Habitation(object):
    def __init__(self, **kwargs):
        self.inhabitants = []

# public structure types
class Storage(_Container, _Structure):
    def __init__(self, **kwargs):
        _Container.__init__(self, **kwargs)
        _Structure.__init__(self,**kwargs)

class Tank(_Container, _Structure):
    def __init__(self, **kwargs):
        _Container.__init__(self, **kwargs)
        _Structure.__init__(self,**kwargs)

class Home(_Habitation, _Structure):
    def __init__(self, **kwargs):
        _Habitation.__init__(self, **kwargs)
        _Structure.__init__(self,**kwargs)

class Baker(_Workplace, _Structure):
    def __init__(self, **kwargs):
        _Workplace.__init__(self, **kwargs)
        _Structure.__init__(self,**kwargs)

class Farm(_Workplace, _Structure):
    def __init__(self, **kwargs):
        _Workplace.__init__(self, **kwargs)
        _Structure.__init__(self,**kwargs)


