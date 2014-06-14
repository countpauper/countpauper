import space as _space

space = _space.Space()
system = _space.System()
space.systems.append(system)
star = _space.Star()
system.stars.append(star)
planet = _space.Planet()
star.bodies.append(planet)


import city as _city
city = _city.City()
planet.cities.append(city)

import definition as _def

workers = { 
    _def.Profession(name='cook') : 1,
    _def.Profession(name='farmer') : 1,
    _def.Profession(name='worker') : 1
}

for type in workers:
    city.population.workers.append(_city.WorkForce(city, type, workers[type]))

structures = { 
    _def.Storage(name='store', space=100) : 1,
    _def.Workplace(name='baker', type='kitchen', space=2) : 1,
    _def.Workplace(name='farm', type='farm', space=5) : 1,
    _def.House(name='house', space=4): 1
}

for type in structures:
    city.infra.append(_city.Structure(city, type, structures[type]))


bread= _def.Supply(name='bread', type='food', storage = 'storage', volume=1)
wheat= _def.Material(name='wheat', type='grain', storage = 'storage', volume=1)

baking = _def.Process()