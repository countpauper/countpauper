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

cook = _spec.Profession(_space.city.agent.Population, 'cook')
farmer = _spec.Profession(_space.city.agent.Population, 'farmer')
worker = _spec.Profession(_space.city.agent.Population, 'worker')
engineer = _spec.Profession(_space.city.agent.Population, 'engineer')

workers = { 
    cook : 2,
    farmer : 1,
    worker : 1,
    engineer : 1,
}

for type, amount in workers.iteritems():
    masterdam.actors.append(type.create(amount))

structures = { 
    _spec.Structure(name='store', type=_space.city.building.Storage,space=250) : 2,
    _spec.Structure(name='tank', type=_space.city.building.Tank, space=2000) : 1,
    _spec.Structure(name='baker', type=_space.city.building.Baker, space=1) : 2,
    _spec.Structure(name='farm', type=_space.city.building.Farm, space=1) : 1,
    _spec.Structure(name='house', type=_space.city.building.Home, space=1): 1,
    _spec.Structure(name='well', type=_space.city.building.Well, space=1): 1,
}

for structure in structures:
    masterdam.infra.append(structure.type(location=masterdam, specification=structure, amount=structures[structure]))

bread = _spec.Product(name='bread', type=_space.item.Food, storage=_space.city.building.Storage, volume=1)
wheat = _spec.Product(name='wheat', type=_space.item.Grain, storage=_space.city.building.Storage, volume=1)
water = _spec.Product(name='H2O', type=_space.item.Water, storage=_space.city.building.Tank, volume=1)
stock = {
    bread : 4 * 5 * 10,
    wheat : 0,
    water : 1000,
}

baking = _spec.Recipe(duration=Day(1), materials={_space.item.Grain:15}, product={bread:16}, facilities=_space.city.building.Baker, professional=cook)
farming = _spec.Recipe(duration=Cycle(1), materials={_space.item.Water:1024}, product={wheat:256}, facilities=_space.city.building.Farm, professional=farmer)
welling = _spec.Recipe(duration=Day(1), materials={}, product={water:200}, facilities=_space.city.building.Well, professional=engineer)

organizations = [ _space.city.organization.Guild(baking), _space.city.organization.Guild(farming), _space.city.organization.Guild(welling) ]
for org in organizations:
    masterdam.actors.append(org)

 
for product, amount, producer in zip(stock.keys(), stock.values(), organizations):
    item = product.type(specification=product, amount=amount)
#    masterdam.reserve(item.storage, item.volume * amount)
#    masterdam.store(product)
#    masterdam.sell(producer,item, 100) 

#masterdam.order(baking, 1)
#masterdam.order(farming, 1)


