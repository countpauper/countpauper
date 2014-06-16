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

import definition as _def

workers = { 
    _def.Profession(name='cook') : 1,
    _def.Profession(name='farmer') : 1,
    _def.Profession(name='worker') : 1
}

for type in workers:
    masterdam.population.workers.append(_space.city.WorkForce(masterdam, type, workers[type]))

structures = { 
    _def.Building(name='store', type=_space.city.structure.Storage,space=100) : 1,
    _def.Building(name='baker', type=_space.city.structure.Baker, space=2) : 1,
    _def.Building(name='farm', type=_space.city.structure.Farm, space=5) : 1,
    _def.Building(name='house', type=_space.city.structure.Home, space=4): 1
}

for structure in structures:
    masterdam.infra.append(structure.type(location=masterdam, specification=structure, amount=structures[structure]))

stock = {
    _def.Product(name='bread', type=_space.item.Food, storage=_space.city.structure.Storage, volume=1) : 10,
    _def.Product(name='wheat', type=_space.item.Grain, storage=_space.city.structure.Storage, volume=1) : 10,
}
  
for item in stock:
    product = item.type(specification=item, amount=stock[item])
    masterdam.store(product)

baking = _def.Process()