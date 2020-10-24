<drac2>
#Downtime code run after {code} to apply cusomizable updates
# input
items=@I@
consumed=@C@
game_data=load_json(get_gvar('c2bd6046-90aa-4a2e-844e-ee736ccbc4ab'))

#output
fields =''
fields += f' -f "{typeof(consumed)}"|"{consumed.items()}" '

bag_update=False
bag_var='bags'
if exists(bag_var):
	bag=load_json(get(bag_var))
else:
	bag=None

items_log = {}
for (i,q_str) in items.items():
	for (item, q_str) in items.items():
		# split all identifiers, by replacing all operators "+-*/" with spaces and removing all spaces and tabs
		vars = [v for v in q_str.translate("".maketrans("+-*/", "    ", " \t\n")).split() if v.isidentifier()]
		# replace all these identifiers with local variables
		for var in vars:
			q_str = q_str.replace(var, str(get(var, 0)))
	# Compute the quantity formula, with identifiers replaced with values
	q = roll(q_str)
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
	set_cvar(bag_var, dump_json(bag))


for (cc,q_str) in consumed.items():
	# same routine as for items, replace identifiers with variables
	vars = [v for v in q_str.translate("".maketrans("+-*/", "    ", " \t\n")).split() if v.isidentifier()]
	for var in vars:
		q_str = q_str.replace(var, str(get(var, 0)))
	q=roll(q_str)
	if not character().cc_exists(cc):
		cc=cc.lower()
		if not cc_exists(cc):
			fields += f'-f "{cc}[{-q}]"|"Not Available"|inline '
			continue
	character().mod_cc(cc,-q)
	fields+=f'-f "{cc}[{-q}]"|"{cc_str(cc)}"|inline '
return fields
</drac2>