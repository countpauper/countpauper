<drac2>
c=character()
bags=load_json(get('bags','{}'))
arg_str='&*&'
args=argparse(arg_str)

# get configuration in priority order: server, character, global
var_name='Shoot'
server_data=load_json(get_svar(var_name,'{}'))
char_data=load_json(get(var_name,'{}'))
game_data=load_json(get_gvar('c2bd6046-90aa-4a2e-844e-ee736ccbc4ab'))
ready_bags=server_data.get('equipment',[])+char_data.get('equipment',[])+game_data.get('equipment_bags',[])
weapons=server_data.get('weapons', char_data.get('weapons', game_data.get('ranged_weapons')))
ammo_containers=server_data.get('ammo', char_data.get('ammo', game_data.get('ammo_bags')))

# explicit argument attacks
attack_options=None
if len(arg_str):
	first_arg='%1%'
	if first_arg[0]!='-':
		attack_options=[a.name for a in c.attacks if first_arg.lower() in a.name.lower()]
# else all recognized ranged attacks
if attack_options is None:
	attack_options = [a.name for a in c.attacks if any([weapon.lower() in a.name.lower() for weapon in weapons.keys()])]
if not attack_options:
	return f"""echo No known ranged attack listed. You can add some on your sheet, dashboard or using `!a add`.
If the attack name contains one of {", ".join(weapons.keys())} it will be automatically selected. 
Altenatively you can specify an existing attack as the first argument to throw it."""

original_attacks = attack_options
# auto resolve attack matching the wielded weapon
ignore = args.last('i')
equipment_bags = []
if bags:
	# check all ready_bags if they exist
	for rb in ready_bags:	# ordered so equipment bags are priotized
		equipment_bags+=[b for b in bags if b[0].lower()==rb.lower()]
	if not equipment_bags and not ignore:
		return f'echo No valid equipped weapon bags. Create a bag using `!bag $ "{ready_bags[0]}"`. Other valid bag names are: {", ".join(ready_bags[1:])}'

	# filter out all attack options matching (partial and case insentive) with items that are equipped
	if not ignore:
		equipped_items = []
		for eb in equipment_bags:
			equipped_items += eb[1].keys()
		attack_options = [ao for ao in attack_options if any([ao.lower() in item.lower() for item in equipped_items])]

	equipment_bags = [eb[0] for eb in equipment_bags]
	if not attack_options:
		return f'echo No valid attack options. Make sure you equip a matching weapon using `!bag {equipment_bags[0]} + "{original_attacks[0]}"` '

if len(attack_options)>1:
	return f'echo Too many attack options. Use `!{ctx.alias} <attack> [arguments]`, pick attack from: {", ".join(attack_options)}'

attack_name=attack_options[0]

ammo_name = args.last('ammo')
specific_ammo=True
if not ammo_name:
	# partial case insensitive weapon to attack matches, to allow for +1 weapons
	ammo_matches = [weapon.get('ammo') for (weapon_name, weapon) in weapons.items() if weapon_name.lower() in attack_name.lower()]
	specific_ammo = False
	if ammo_matches:
		ammo_name = ammo_matches[0]
	else:
		ammo_name= attack_name	# thrown weapons

if not ammo_name:	# listed weapon with specifically no ammo
	return f'a {attack_name} {arg_str}'

ammo_plural = ammo_name+('s' if ammo_name[-1]!='s' and ammo_name[-1:].isalpha() else '')
ammo_names = [ammo_name, ammo_plural]
ammo_names+=[a.lower() for a in ammo_names]

ammo_split = ammo_name.split('+',maxsplit=1)
bonus_str=''
if len(ammo_split)>1:
	bonus_postfix = ammo_split[1]
	if f'+{bonus_postfix}' not in attack_name and bonus_postfix.isdigit():
		bonus = int(bonus_postfix)
		bonus_str=f'magical  -b {bonus} -d {bonus}'

ammo_bag_names=[ammo_container for (contained_ammo,ammo_container) in ammo_containers.items() if contained_ammo.lower() in ammo_name.lower()]
ammo_bag_names+=[eb for eb in equipment_bags]
# compute the needed ammo
ammount =  len(args.get('t'))
if ammount==0:
	ammount=1
ammount *= int(args.last('rr',1))

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
		if specific_ammo:
			return f'echo There are not enough {ammo_plural} left in your {bag_name.lower()}. You can add some using `!bag "{bag_name}" + {ammount-used} "{ammo_name}"`.'
		else:
			return f'echo There are not enough mundane {ammo_plural} left in your {bag_name.lower()}. You can add some using `!bag "{bag_name}" + {ammount-used} "{ammo_name}"`. To specify magic ammo use the `-ammo "<Item>"` argument.'
	field = f'-f "{bag_name}|{total-used} [{-used}] {ammo_name.lower()}{"s" if used > 1 else ""}"'
	# apply the changes
	character().set_cvar('bags', dump_json(bags))
else:
	if ammo_bag_names:
		bag_name=ammo_bag_names[0]
	else:
		bag_name='Ammo'
	field = f'-f "{bag_name}|{-ammount}"'

return f'a {attack_name} {arg_str} {bonus_str} {field}'
</drac2>