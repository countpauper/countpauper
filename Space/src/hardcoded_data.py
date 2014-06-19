import space as _space
from space.time import Hour, Day, Cycle

space = _space.Space()
system = _space.System()
space.systems.append(system)
star = _space.Star()
system.stars.append(star)
planet = _space.Planet()
star.bodies.append(planet)


masterdam = _space.city.City("Masterdam")
planet.cities.append(masterdam)

import specification as _spec

cook = _spec.Profession(name='cook')
farmer = _spec.Profession(name='farmer')
worker = _spec.Profession(name='worker')
engineer = _spec.Profession(name='engineer')

workers = { 
    cook : 2,
    farmer : 1,
    worker : 1,
    engineer : 1,
}

for type in workers:
    masterdam.population.workers.append(_space.city.Group(masterdam, type, workers[type]))

structures = { 
    _spec.Building(name='store', type=_space.city.structure.Storage,space=1000) : 10,
    _spec.Building(name='tank', type=_space.city.structure.Tank, space=10000) : 1,
    _spec.Building(name='baker', type=_space.city.structure.Baker, space=1) : 2,
    _spec.Building(name='farm', type=_space.city.structure.Farm, space=1) : 1,
    _spec.Building(name='house', type=_space.city.structure.Home, space=1): 1,
    _spec.Building(name='well', type=_space.city.structure.Well, space=1): 1,
}

for structure in structures:
    masterdam.infra.append(structure.type(location=masterdam, specification=structure, amount=structures[structure]))

bread = _spec.Product(name='bread', type=_space.item.Food, storage=_space.city.structure.Storage, volume=1)
wheat = _spec.Product(name='wheat', type=_space.item.Grain, storage=_space.city.structure.Storage, volume=1)
water = _spec.Product(name='H2O', type=_space.item.Water, storage=_space.city.structure.Tank, volume=1)
stock = {
    bread : 4 * 5 * 10,
    wheat : 0,
    water : 1000,
}
  
for item in stock:
    product = item.type(specification=item, amount=stock[item])
    masterdam.store(product)

baking = _spec.Recipe(duration=Day(1), materials={_space.item.Grain:15}, product={bread:15}, facilities=_space.city.structure.Baker, professional=cook)
farming = _spec.Recipe(duration=Cycle(1), materials={_space.item.Water:1000}, product={wheat:200}, facilities=_space.city.structure.Farm, professional=farmer)
welling = _spec.Recipe(duration=Day(1), materials={}, product={water:200}, facilities=_space.city.structure.Well, professional=engineer)

#masterdam.order(baking, 1)
#masterdam.order(farming, 1)

masterdam.government.responsibilities = baking, farming, welling
