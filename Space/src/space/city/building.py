from specification import Product, Resource

class StorageException(Exception):
    pass

class OccupancyException(Exception):
    pass
# private abstract base classes

class Building(Resource):
    def __init__(self, location, specification, amount):
        super(Building, self).__init__(specification, amount)
        self.location = location
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

    def reserve(self, amount):
        raise StorageException("Not a container")

    def stock(self, product):
        return 0

    def space(self):
        return 0

    def retrieve(self, product, amount):
        raise StorageException("Not a container")

class _Container(object):
    def __init__(self, **kwargs):
        self.contents = []
        self.reserved = 0

    def reserve(self, space):
        if space > self.space():
            raise StorageException("Not enough space in {0} for reservation", self)
        self.reserved += space

    def store(self, item):
        # TODO: check space, current container
        if not isinstance(self, item.specification.storage):
            raise StorageException("Wrong storage type {0} for {1}", self, item)
        if item.volume() > self.reserved:
            raise StorageException("No reservation in {0} for {1}", self, item)
        self.reserved -= item.volume()      
        same_item = self.find(item.specification)
        if same_item:
            same_item[0].stack(item)
        else:
            same_item = item.specification.create(amount=0)
            self.contents.append(same_item)
            same_item.stack(item)

    def find(self, product):
        if product in self.contents:
            return product
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

    def _used_space(self):
        return sum([item.volume() for item in self.contents])

    def space(self):
        space = self.specification.space * self.amount
        space -= self._used_space()
        return space - self.reserved

class _Workplace(object):
    def __init__(self, **kwargs):
        pass

class _Habitation(object):
    def __init__(self, **kwargs):
        self.inhabitants = []

# public structure types
class Storage(_Container, Building):
    def __init__(self, **kwargs):
        _Container.__init__(self, **kwargs)
        Building.__init__(self,**kwargs)

class Tank(_Container, Building):
    def __init__(self, **kwargs):
        _Container.__init__(self, **kwargs)
        Building.__init__(self,**kwargs)

class Home(_Habitation, Building, _NotContainer):
    def __init__(self, **kwargs):
        _Habitation.__init__(self, **kwargs)
        Building.__init__(self,**kwargs)
        _NotContainer.__init__(self, **kwargs)

class Baker(_Workplace, Building, _NotContainer):
    def __init__(self, **kwargs):
        _Workplace.__init__(self, **kwargs)
        Building.__init__(self,**kwargs)
        _NotContainer.__init__(self, **kwargs)

class Farm(_Workplace, Building, _NotContainer):
    def __init__(self, **kwargs):
        _Workplace.__init__(self, **kwargs)
        Building.__init__(self,**kwargs)
        _NotContainer.__init__(self, **kwargs)

class Well(_Workplace, Building, _NotContainer):
    def __init__(self, **kwargs):
        _Workplace.__init__(self, **kwargs)
        Building.__init__(self,**kwargs)
        _NotContainer.__init__(self, **kwargs)


