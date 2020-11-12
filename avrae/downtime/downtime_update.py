<drac2>
#Downtime code run after {code} to apply customizable updates, with variables set in code blocks from json

game_data=load_json(get_gvar('c2bd6046-90aa-4a2e-844e-ee736ccbc4ab'))
char = character()
me = combat().me if combat() else None

#output
fields =' '

# Update downtime variable store with changedvars
changedvars=@V@
update_var=False
downtime_var='Downtime'
downtime=load_json(get(downtime_var,'{}'))
storedvar=downtime.get('var',{})
for (changed,q) in changedvars.items():
	if typeof(q)=='str':
		# split all identifiers, by replacing all operators "+-*/" with spaces and removing all spaces and tabs
		vars = [v for v in q.replace('//',' ').translate("".maketrans("+-*/", "    ", " \t\n")).split() if v.isidentifier()]
		# replace all these identifiers with previously stored or local variables
		for var in vars:
			q = q.replace(var, str(storedvar.get(var,get(var, 0))))
		q = roll(q)
	if q is None:
		storedvar.pop(changed)
	else:
		storedvar[changed] = q
	update_var = True
downtime['var'] = storedvar
if update_var:
	character().set_cvar(downtime_var,dump_json(downtime))


# update bags with items
items=@I@
bag_update=False
bag_var='bags'
if exists(bag_var):
	bag=load_json(get(bag_var))
else:
	bag=None
items_log = {}
for (item,q) in items.items():
	if typeof(q)=='str':
		# split all identifiers, by replacing all operators "+-*/" with spaces and removing all spaces and tabs
		vars = [v for v in q.replace('//',' ').translate("".maketrans("+-*/", "    ", " \t\n")).split() if v.isidentifier()]
		# replace all these identifiers with set or local variables
		for var in vars:
			q = q.replace(var, str(storedvar.get(var, get(var, 0))))
		# Compute the quantity formula, with identifiers replaced with values
		q = roll(q)
	if q:
		if bag:
			# add or remove items that are already owned and find preferred bag to add to
			special_bags = game_data.get('special_bags', [])
			pref_bag = None
			for b in bag:
				if item in b[1]:
					bag_items = b[1][item]
					item_delta = max(-bag_items, q)
					q -= item_delta
					items_log[item] = items_log.get(item, 0) + item_delta
					b[1][item] = bag_items + item_delta
				if not pref_bag and b[0].lower() not in special_bags:
					pref_bag = b
			if q > 0:
				if not pref_bag:
					bag.append(["Held", {}])
					pref_bag = bag[-1]
				pref_bag[1][item] = pref_bag[1].get(item, 0) + q
				items_log[item] = items_log.get(item, 0) + q

			bag_update = True
		else:
			items_log[item] = q

if items_lost:= ",".join([(f'{-q} x {i}' if q<-1 else i) for (i, q) in items_log.items() if q<0]):
	fields+=f'-f "Items Lost"|"{items_lost}"|inline '
if items_gained:= ",".join([(f'{q} x {i}' if q>1 else i) for (i, q) in items_log.items() if q>0]):
	fields+=f'-f "Items Gained"|"{items_gained}"|inline '
if bag_update:  # update bag at end to avoid one sided item loss
	char.set_cvar(bag_var, dump_json(bag))

# update consumable counters
consumed=@C@
for (cc,q) in consumed.items():
	# same routine as for items, replace identifiers with values
	if typeof(q) == 'str':
		vars = [v for v in q.replace('//',' ').translate("".maketrans("+-*/", "    ", " \t\n")).split() if v.isidentifier()]
		for var in vars:
			q = q.replace(var, str(storedvar.get(get(var, 0))))
		# negate the string, so it shows up right in the field
		if q[0]=='-':
			q=q[1:]
		else:
			q='-'+q
		r=vroll(q)
		q=r.total
	else:
		r=None

	if not char.cc_exists(cc):
		cc=cc.lower()
		if not cc_exists(cc):
			fields += f'-f "{cc}[{r.dice if r else q}]"|"Not Available"|inline '
			continue
	char.mod_cc(cc,r.total)
	fields+=f'-f "{cc}[{r.dice if r else q}]"|"{cc_str(cc)}"|inline '

# update character stats with modified
modified=@M@
resists=[r.dtype for r in char.resistances.resist]
immunes=[i.dtype for i in char.resistances.immune]
vulnerables=[v.dtype for v in char.resistances.vuln]

if me:
	for e in me.effects:
		resists+=e.effect.get('resist',[])
		immunes+=e.effect.get('immune',[])
		vulnerables+=e.effect.get('vulnerable',[])	#hypothetical effect

dtype_table={"*0":immunes,"//2":resists,"*2":vulnerables}
for (mod, q) in modified.items():
	if typeof(q)=='str':
		# again replace identifiers with values
		vars = [v for v in q.replace('//',' ').translate("".maketrans("+-*/", "    ", " \t\n")).split() if v.isidentifier()]
		for var in vars:
			q = q.replace(var, str(storedvar.get(var, get(var, 0))))
		r = vroll(q)
		q=r.total
	else:
		r=None
	if mod=='hp':
		for (multiplier,dtypes) in dtype_table.items():
			for dtype in dtypes:
				q = q.replace(f'[{dtype}]', f'{multiplier}[{dtype}]')
		char.modify_hp(q,overflow=False)
		fields += f'-f "Hitpoints [{r.dice if r else q}]|{char.hp_str()}|inline" '
	elif mod=='thp':
		char.set_temp_hp(max(char.temp_hp,r.total,0))
		fields += f'-f "Temporary HP [{r.dice if r else q}]|{char.temp_hp}|inline" '

return fields
</drac2>