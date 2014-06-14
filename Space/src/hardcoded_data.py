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
    _def.Storage(name='store', space=100) : 1,
    _def.Workplace(name='baker', type='kitchen', space=2) : 1,
    _def.Workplace(name='farm', type='farm', space=5) : 1,
    _def.House(name='house', space=4): 1
}

for type in structures:
    masterdam.infra.append(_space.city.structure.Structure(masterdam, type, structures[type]))


bread= _def.Supply(name='bread', type='food', storage = 'storage', volume=1)
wheat= _def.Material(name='wheat', type='grain', storage = 'storage', volume=1)

baking = _def.Process()