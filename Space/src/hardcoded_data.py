import space as _space

space = _space.Space()
system = _space.System()
space.systems.append(system)
star = _space.Star()
system.stars.append(star)
planet = _space.Planet()
star.bodies.append(planet)


masterdam = _space.city.City()
planet.cities.append(masterdam)

import specification as _spec

cook = _spec.Profession(name='cook')
farmer = _spec.Profession(name='farmer')
worker = _spec.Profession(name='worker')

workers = { 
    cook : 1,
    farmer : 1,
    worker : 1
}

for type in workers:
    masterdam.population.workers.append(_space.city.Group(masterdam, type, workers[type]))

structures = { 
    _spec.Building(name='store', type=_space.city.structure.Storage,space=100) : 1,
    _spec.Building(name='tank', type=_space.city.structure.Tank, space=100) : 1,
    _spec.Building(name='baker', type=_space.city.structure.Baker, space=2) : 1,
    _spec.Building(name='farm', type=_space.city.structure.Farm, space=5) : 1,
    _spec.Building(name='house', type=_space.city.structure.Home, space=4): 1
}

for structure in structures:
    masterdam.infra.append(structure.type(location=masterdam, specification=structure, amount=structures[structure]))

bread = _spec.Product(name='bread', type=_space.item.Food, storage=_space.city.structure.Storage, volume=1)
wheat = _spec.Product(name='wheat', type=_space.item.Grain, storage=_space.city.structure.Storage, volume=1)
water = _spec.Product(name='H2O', type=_space.item.Water, storage=_space.city.structure.Tank, volume=1)
stock = {
    bread : 10,
    wheat : 10,
    water : 10,
}
  
for item in stock:
    product = item.type(specification=item, amount=stock[item])
    masterdam.store(product)

baking = _spec.Recipe(duration=1, materials={_space.item.Grain:1}, product={bread:1}, facilities=_space.city.structure.Baker, professional=cook)
farming = _spec.Recipe(duration=10, materials={_space.item.Water:1}, product={wheat:10}, facilities=_space.city.structure.Farm, professional=farmer)

masterdam.order(baking, 1)
masterdam.order(farming, 1)
