from item import Item
from container import Container

# TODO: define item types in a yaml hierarchy with the roots have Item as base and the leaves their parent as base
Food = type('Item', (Item,), dict())
Bread = type('Food', (Food,), dict(weight=1, name='Bread'))
Shelf = type('Shelf', (Container,),  dict(weight=8, name='Shelf', door=False, lock=False))

items_types=dict(bread=Bread, shelf=Shelf)
