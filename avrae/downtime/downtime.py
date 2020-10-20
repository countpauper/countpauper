tembed <drac2>
# TODO data https://www.dndbeyond.com/sources/xgte/downtime-revisited#undefined
#   Sub commnd xge to enble, phb for phb downtime nd dmg for dmg downtime stuff set in svar
# besides code, can have 'item' (auto added to bag) and other stuff
#	- item name added to description automatically like scroll found reading
#    - quantity for item, can be roll, roll string can contain var identifiers set by {code} and other cvars or quantity:numnber or quantity:str
#	- can also be gp or sp or cp, simply added to coin pouch as item, recognize by name? or 'coin' or find existing item to increment, else default bag
#	- also lose item and coins as bad stuff, if don't have, clip count? no result?
# TODO: table['items'], str type split to array, array items, to counted dict (see craft)
# image thumb per activity
# image per result (overrides or -image and activity is -thumb or img= and thumb= )
# exhaustion (or in general consumable +-) as a result option (nb avrae dev !exhaustion alias with cc ties into !lr already, just need to be compatible)
# tool/skill/item precondition (tool is both) (ownership and precondition) (fishing: tackle, reading: book, hunting bow or trap, burglary=crowbar, working with tool: tool)
# "Inspiration" (just a cc?) snippet & cc, inspiration as a result option (NB inspiration effect and snippet used for bard)
# relation added to !relation, need a table for location?(channel)

# and make sure most of that is possible
sv = load_json(get_svar('downtime','{}'))
gv= sv.get('activities',['b566982b-c7f8-4ea4-af47-4fd82623b335'])
data={}
for dgv in gv:
	data.update(load_json(get_gvar(dgv)))

# parse arguments
arg="&*&".lower()
if arg=='?' or arg.lower()=='help' or arg=='' or not arg in data:
	return f'-title "{name} spends their free time getting help." -desc "`!downtime <activity>` where <activity> is one of {", ".join(data.keys())}."'

# recreate downtime consumable with current config, keeping value
char=character()
cc_max = sv.get('max',1)
cc_max = int(cc_max) if str(cc_max).isdigit() else None
cc_reset=sv.get('reset','long').lower()
ccn='Downtime'
if char.cc_exists(ccn):
	cc_val=char.get_cc(ccn)
	char.delete_cc(ccn)
else:
	cc_val=None
if cc_max>0:
	char.create_cc(ccn, maxVal=cc_max, reset=cc_reset)
	if cc_val is not None:
		char.set_cc(ccn,cc_val)

# check pre conditions
activity=data[arg]
particle=activity.get('particle',arg+'ing')
downtime=char.get_cc(ccn)
if not downtime:
	return f'-title "{name} doesn\'t have time to {arg}" -desc "You have no more downtime left." -f Downtime|"{cc_str(ccn)}"|inline'

# Preconditions met, roll for results
game_data=load_json(get_gvar('c2bd6046-90aa-4a2e-844e-ee736ccbc4ab'))
title=f'{name} spends their free time {particle}. '
node = activity
code,desc,fields='','',''
items={}
while node:
	code+=node.get('code','')
	desc+=node.get('text','')
	table=node.get('table')
	# TODO: str items, split to array, array items, to counted dict (see craft)
	for (i,q) in  node.get('items',{}).items():
		items[i]=(f'{items[i]}+' if i in items else '')+str(q)
	if table:
		if roll_expr:=table.get('roll'):
			r = vroll('+'.join([str(get(e)) + f"[{e[:3]}]" if e.isidentifier() else e for e in roll_expr.split('+')]))
			fields += f'-f Roll|"{r.full}"|inline '
		elif skill:=table.get('skill'):
			r = vroll( character().skills[skill].d20())
			fields += f'-f {game_data["skills"][skill]}|"{r.full}"|inline '
		else:
			r = vroll('1d100')
			fields += f'-f Roll|"{r.full}"|inline '
		options=[int(k) for k in table.keys() if (k.isdigit() or (k[0]=='-' and k[1:].isdigit())) and int(k) <= r.total]
		if options:
			node=table[str(max(options))]
		else:
			node=table.get('default',None)
	else:
		node=None

# Apply the changes to the character
bag_var='bags'
if exists(bag_var):
	bag=load_json(get(bag_var))
else:
	bag=None

#TODO inject code here using {'{'*2 and whatever if possible, to set vars
items_desc={}
bag_update=False
for (item,q_str) in items.items():
	# split all identifiers, by replacing all operators "+-*/" with spaces and removing all spaces and tabs
	vars=[v for v in q_str.translate("".maketrans("+-*/","    "," \t\n")).split() if v.isidentifier()]
	# replace all these identifiers with local variables
	for var in vars:
		q_str.replace(var,str(get(var,0)))
	q = roll(q_str)
	if q:
		if bag:
			# add or remove items that are already owned and find prefered add bag
			special_bags=game_data.get('special_bags',[])
			pref_bag=None
			for b in bag:
				if item in b[1]:
					bag_items=b[1][item]
					item_delta=max(-bag_items,q)
					q-=item_delta
					items_desc[item] = items_desc.get(item, 0) + item_delta
					b[1][item]=bag_items+item_delta
				if not pref_bag and b[0] not in special_bags:
					pref_bag=b
			if q>0:
				if not pref_bag:
					bag.append(["Held",{}])
					pref_bag=bag[-1]
				pref_bag[1][item] = pref_bag[1].get(item,0)+q
				items_desc[item] = items_desc.get(item, 0) + q

			bag_update=True
		else:
			items_desc[item] = items_desc.get(item,0)+q

items_desc=",".join([f'{q}x{i}' for (i,q) in items_desc.items()])
#fields+=f'-f Items|"{items_desc}" '
if bag_update:	# update bag at end to avoid item loss TODO: add to end of code instead and deltas with rolls
	set_cvar(bag_var, dump_json(bag))

char.mod_cc(ccn,-1,True)
fields+=f'-f Downtime|"{cc_str(ccn)}"|inline '
if not desc:
	desc='Nothing happens.'
return f'-title "{title}" {code} -desc "{desc}" '+fields
</drac2>