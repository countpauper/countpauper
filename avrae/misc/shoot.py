<drac2>
c=character()
bags=load_json(get('bags','{}'))
arg_str='&*&'
args=argparse(arg_str)

# prioritized bags that could contain ranged weapons
ready_bags=["Equipped","Equipment","Wielded","Held","Worn","Wearing"]
weapons={	# todo: json svar or default game_data gvar
	# todo: attack -> ammo (-ammo to override)
	"Longbow": "Arrow",
	"Shortbow": "Arrow",
	"Blowgun": "Blowgun Needle",
	"Crossbow":"Crossbow Bolt",
	"Sling": "Sling bullet"
}
weapons={weapon.lower():ammo for (weapon,ammo) in weapons.items()}

ammo_containers={
	"Arrow":"Quiver",
	"Needle":"Pouch",
	"Crossbow Bolt":"Case, Crossbow Bolt",
	"Sling bullets":"Pouch"
}

# explicit argument attacks
attack_options=None
if len(arg_str):
	first_arg='%1%'
	if first_arg[0]!='-':
		attack_options=[a.name for a in c.attacks if first_arg.lower() in a.name.lower()]
# else all recognized ranged attacks
if attack_options is None:
	attack_options = [a.name for a in c.attacks if any([weapon in a.name.lower() for weapon in weapons.keys()])]
if not attack_options:
	return 'echo No ranged attack options. Add some on your sheet and `!update` or on the dashboard or by using `!a add`.'

original_attacks = attack_options
# auto resolve attack matching the wielded weapon
equipment_bags = []
if bags:
	# check all ready_bags if they exist
	for rb in ready_bags:	# ordered so equipment bags are ordered
		equipment_bags+=[b for b in bags if b[0].lower()==rb.lower()]
	if not equipment_bags:
		return f'echo No valid equipped weapon bags. Create a bag using `!bag $ "{ready_bags[0]}"`. Other valid bag names are: {", ".join(ready_bags[1:])}'

	# filter out all attack options matching (case insentive) with items that are equipped
	attack_options = [ao for ao in attack_options if any([ao.lower() in [ek.lower() for ek in eb[1].keys()] for eb in equipment_bags])]
	equipment_bags = [eb[0] for eb in equipment_bags]
	if not attack_options:
		return f'echo No valid attack options. Make sure you equip a matching weapon using `!bag {equipment_bags[0]} + "{original_attacks[0]}"` '

if len(attack_options)>1:
	return f'echo Too many attack options. Use `!{ctx.alias} <attack> [arguments]`, pick attack from: {", ".join(attack_options)}'

attack_name=attack_options[0]

ammo_name = args.last('ammo')
if not ammo_name:
	# partial case insensitive weapon to attack matches, to allow for +1 weapons
	ammo_matches = [ammo_name for (weapon_name, ammo_name) in weapons.items() if weapon_name in attack_name.lower()]
	if ammo_matches:
		ammo_name = ammo_matches[0]
	else:
		ammo_name= attack_name	# thrown weapons

if not ammo_name:	# listed weapon with specifically no ammo
	return f'a {attack_name} {arg_str}'

ammo_plural = ammo_name+('s' if ammo_name[-1]!='s' and ammo_name[-1:].isalpha() else '')
ammo_names = [ammo_name, ammo_plural]
ammo_names+=[a.lower() for a in ammo_names]



ammo_bag_names=[ammo_container for (contained_ammo,ammo_container) in ammo_containers.items() if contained_ammo.lower() in ammo_name.lower()]
ammo_bag_names+=[eb for eb in equipment_bags]
# compute the needed ammo
ammount =  len(args.get('t'))
if ammount==0:
	ammount=1
ammount *= int(args.last('rr',1))

# TODO: add bonuses of +1-3 ammo automatically

if bags:
	# collect all existing ammo bags, sorted by preference
	ammo_bags=[]
	for ammo_bag_name in ammo_bag_names:
		ammo_bags+=[b for b in bags if b[0].lower()==ammo_bag_name.lower()]

	if not ammo_bags:
		err("No ammo containers found, there should at least be equipment bags setup.")
	bag_name=ammo_bags[0][0]

	# go over all ammmo bags. Remove sufficient ammo and count how many total there were
	used,total=0,0
	for ammo_bag in ammo_bags:
		ammo_items = ammo_bag[1]
		bag_items = [item for item in ammo_items.keys()]	# copy of keys to adjust ammo_bag during iteration
		for item in bag_items:
			q = ammo_items.get(item,0)
			if item in ammo_names:
				total+=q
				use = min(q,ammount-used)
				if use>0:
					if remaining:=q-use:
						ammo_items[item] = remaining
					else:
						ammo_items.pop(item)
				used+=use
	if used<ammount:
		return f'echo There are not enough {ammo_plural} left in your {bag_name.lower()}. You can add some using `!bag {bag_name} + {ammount-used} "{ammo_name}"`'
	field = f'-f "{bag_name}|{total-used} [{-used}] {ammo_name.lower()}{"s" if used > 1 else ""}"'
	# apply the changes
	character().set_cvar('bags', dump_json(bags))
else:
	if ammo_bag_names:
		bag_name=ammo_bag_names[0]
	else:
		bag_name='Ammo'
	field = f'-f "{bag_name}|{-ammount}"'

return f'a {attack_name} {arg_str} {field}'
</drac2>