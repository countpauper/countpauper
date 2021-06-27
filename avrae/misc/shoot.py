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
weapons=server_data.get('weapons', char_data.get('weapons', game_data.get('weapons')))
ammo_containers=server_data.get('ammo', char_data.get('ammo', game_data.get('ammo_bags')))

# ensure all configuration is lower case
ready_bags=[rb.lower() for rb in ready_bags]
weapons={w.lower():wd for w,wd in weapons.items() if wd.get('range')}
ammo_containers={ac.lower():ad for ac,ad in ammo_containers.items()}

# explicit argument attacks
attack_options=None
if len(arg_str):
	first_arg='%1%'
	# if the first argument is a known attack argument/snippet, don't use it for partial matching
	known_snippets=['adv','dis','ea','magical','max','crit','nocrit','hit','miss','sneak','tides','curse','foe','gwm','gfb','bb','steady','rage','hex','dmi','ammo']
	if first_arg[0]!='-' and not first_arg in known_snippets:
		attack_options=[a.name for a in c.attacks if first_arg.lower() in a.name.lower()]
# else all recognized ranged attacks
if attack_options is None:
	attack_options = [a.name for a in c.attacks if any([weapon.lower() in a.name.lower() for weapon in weapons.keys()])]
if not attack_options:
	return f"""echo No known ranged attack listed. You can add some on your sheet, dashboard or using `!a add`.
If the attack name contains one of {", ".join(weapons.keys())} it will be automatically selected. 
Altenatively you can specify an existing attack as the first argument to throw it."""

original_attacks = attack_options
# auto select attack matching the wielded weapon
equipment_bags = [b for b in bags if b[0].lower() in ready_bags]
if equipment_bags and len(attack_options)>1:

	# filter out all attack options matching (partial and case insentive) with items that are equipped
	equipped_items = []
	for _,eb in equipment_bags:
		equipped_items+=eb.keys()
	equipped_weapons = [ao for ao in attack_options if any([ao.lower() in item.lower() for item in equipped_items])]
	# return f'echo attacks {attack_options} bags {equipment_bags} equipped {equipped_items} equipped rnged weapons {equipped_weapons}'
	if len(equipped_weapons) > 1:
		return f'echo Too many equipped weapons Use `!{ctx.alias} <attack> [arguments]`, pick attack from: {", ".join(equipped_weapons)}."'
	elif equipped_weapons:
		attack_options=equipped_weapons

if len(attack_options)>1:
	return f'echo Too many attack options. Use `!{ctx.alias} <attack> [arguments]`, pick attack from: {", ".join(attack_options)}.\nYou can also pre-select by placing your favored ranged weapon in a ready bag like *{" ".join(ready_bags)}*.'

attack_name=attack_options[0]

ammo_name = args.last('ammo')
specific_ammo=True
if not ammo_name:
	# partial case insensitive weapon to attack matches, to allow for +1 weapons
	ammo_matches = [weapon.get('ammo',weapon_name) for (weapon_name, weapon) in weapons.items() if weapon_name.lower() in attack_name.lower()]
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
	if f'+{bonus_postfix}' not in attack_name and bonus_postfix.isdecimal():
		bonus = int(bonus_postfix)
		bonus_str=f'magical  -b {bonus} -d {bonus}'

ammo_bag_names=[ammo_container.lower() for (contained_ammo,ammo_container) in ammo_containers.items() if contained_ammo.lower() in ammo_name.lower()]
# compute the needed ammo
ammount =  len(args.get('t'))
if ammount==0:
	ammount=1
ammount *= int(args.last('rr',1))

if bags:
	# collect all existing ammo bags, sorted by preference
	ammo_bags=[b for b in bags if b[0].lower() in ammo_bag_names]
	ammo_bags+=equipment_bags
	# RAW all containers can be used as part of an action, just prefer the ammo bags, equipment bags, add the rest after
	ammo_bags+=[b for b in bags if not b in ammo_bags]
	bag_name=ammo_bags[0][0]

	used_ammo={}
	total,used=0,0
	# go over all ammmo bags. Remove sufficient ammo and count how many total there were
	for ammo_bag in ammo_bags:
		ammo_items = ammo_bag[1]
		bag_items = [item for item in ammo_items.keys()]	# copy of keys to adjust ammo_bag during iteration
		for item in bag_items:
			q = ammo_items.get(item,0)
			if item in ammo_names:
				total+=q
				use = min(q,ammount-used)
				used+=use
				used_ammo[ammo_bag[0]]=used_ammo.get(ammo_bag[0],0)+use
				if use>0:
					if remaining:=q-use:
						ammo_items[item] = remaining
					else:
						ammo_items.pop(item)
	used=sum(used_ammo.values())
	if used<ammount:
		if specific_ammo:
			return f'echo There are not enough {ammo_plural} left in your {bag_name.lower()}. You can add some using `!bag "{bag_name}" + {ammount-used} "{ammo_name}"`.'
		else:
			return f'echo There are not enough mundane {ammo_plural} left in your {bag_name.lower()}. You can add some using `!bag "{bag_name}" + {ammount-used} "{ammo_name}"`. To specify magic ammo use the `-ammo "<Item>"` argument.'
	used_bags=','.join(used_ammo.keys())
	field = f'-f "{used_bags}|{total-used} [{-used}] {ammo_name.lower()}{"s" if used > 1 else ""}"'
	# apply the changes
	character().set_cvar('bags', dump_json(bags))
else:
	if ammo_bag_names:
		bag_name=ammo_bag_names[0]
	else:
		bag_name='Ammo'
	field = f'-f "{bag_name.title()}|{-ammount}"'

return f'a {attack_name} {arg_str} {bonus_str} {field}'
</drac2>