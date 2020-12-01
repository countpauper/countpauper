<drac2>
# configure for ammunition type, capitalized
ammo_name='Needle'
bag_name='Case'
# Find the ammo bags in the bags character variable
if not exists('bags'):
	err(f'{ammo_name} counting only works after setting up a {bag_name} with `!bag $ {bag_name}`')
ammo_names=[ammo_name, ammo_name+'s']
ammo_names+=[a.lower() for a in ammo_names]
bags = load_json(bags)
ammo_bags = [(b[1],sum([b[1].get(n,0) for n in ammo_names])) for b in bags if b[0].lower() == bag_name.lower()]
if not ammo_bags:
	err(f'You do not own a {bag_name}. Where are you getting that {ammo_name.lower()} from?')

# count the ammo in the ammo bags
ammo_bags = [(b,a) for (b,a) in ammo_bags if a>0]
total_remaining = sum([q for (_,q) in ammo_bags])
if total_remaining<=0:
	err(f'There are no {ammo_name.lower()}s left in any of your {bag_name.lower()}s.')

# remove the ammo from the first ammo bag
ammo_bag = ammo_bags[0][0]
for ammo in ammo_names:
	if remaining:=ammo_bag.get(ammo,0):
		remaining-=1
		total_remaining-=1
		if remaining:
			ammo_bag[ammo]=remaining
		else:
			ammo_bag.pop(ammo)
		break
# apply the change to the chosen ammo_bag
set_cvar("bags", dump_json(bags))
return f'-f "{ammo_name}s Remaining"|{total_remaining}'
</drac2>