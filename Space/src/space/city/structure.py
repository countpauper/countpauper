from specification import Product

class StorageException(Exception):
    pass

class OccupancyException(Exception):
    pass
# private abstract base classes

class _Structure(object):
    def __init__(self, **kwargs):
        self.location = kwargs['location']
        self.specification = kwargs['specification']
        self.amount = kwargs['amount']
        self.occupied = 0

    def available(self):
        return self.amount - self.occupied

    def rent(self, amount):
        if amount<=self.available():
            self.occupied+=amount
        else:
            raise OccupancyException("Not enough {0} buildings available", self)

    def release(self, amount):
        if amount<=self.occupied:
            self.occupied-=amount
        else:
            raise OccupancyException("Not enough {0} buildings rented", self)

class _NotContainer(object):
    def __init__(self, **kwargs):
        pass

    def store(self, item):
        raise StorageException("Not a container")

    def stock(self, product):
        return 0

    def retrieve(self, product, amount):
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
        if isinstance(product,Product):
            return [item for item in self.contents if item.specification==product]
        elif isinstance(product,str):
            return [item for item in self.contents if item.specification.name==product]
        else:
            return [item for item in self.contents if isinstance(item, product)]

    def stock(self, product):
        items = self.find(product)
        return sum([item.amount for item in items])

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

class Home(_Habitation, _Structure, _NotContainer):
    def __init__(self, **kwargs):
        _Habitation.__init__(self, **kwargs)
        _Structure.__init__(self,**kwargs)
        _NotContainer.__init__(self, **kwargs)

class Baker(_Workplace, _Structure, _NotContainer):
    def __init__(self, **kwargs):
        _Workplace.__init__(self, **kwargs)
        _Structure.__init__(self,**kwargs)
        _NotContainer.__init__(self, **kwargs)

class Farm(_Workplace, _Structure, _NotContainer):
    def __init__(self, **kwargs):
        _Workplace.__init__(self, **kwargs)
        _Structure.__init__(self,**kwargs)
        _NotContainer.__init__(self, **kwargs)

class Well(_Workplace, _Structure, _NotContainer):
    def __init__(self, **kwargs):
        _Workplace.__init__(self, **kwargs)
        _Structure.__init__(self,**kwargs)
        _NotContainer.__init__(self, **kwargs)


