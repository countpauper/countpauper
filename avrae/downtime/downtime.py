tembed <drac2>
# TODO data https://www.dndbeyond.com/sources/xgte/downtime-revisited#undefined
#   Sub commnd xge to enble, phb for phb downtime nd dmg for dmg downtime stuff set in svar
# skill override !downtime perform -with drum,adv,nature|adv... < split to list of tool/skill/die, use up one per nested table until empty.
# skill & tool proficiency precondition ?(tool is both) (ownership and precondition)
# temp hp as json/ damage (negative hp)
# "Inspiration" (just a cc?) snippet & cc, inspiration as a result option (NB inspiration effect and snippet used for bard)
# relation added to !relation, need a table for location?(channel)
# CC find in character consumables to be case insensitive?

# and make sure most of that is possible
sv = load_json(get_svar('downtime','{}'))
gv= sv.get('activities',['b566982b-c7f8-4ea4-af47-4fd82623b335'])
if typeof(gv)=='str':
	gv=gv.split(',')
data={}
for dgv in gv:
	data.update(load_json(get_gvar(dgv.strip())))

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

bag_var='bags'
if exists(bag_var):
	bag=load_json(get(bag_var))
	if tool := activity.get('tool'):
		if not any([b[1].get(tool, 0) > 0 for b in bag]):
			return f'-title "{name} doesn\'t have the tools to {arg}" -desc "You need a {tool}."'
else:
	bag=None

# Preconditions met, roll for results
game_data=load_json(get_gvar('c2bd6046-90aa-4a2e-844e-ee736ccbc4ab'))
title=f'{name} spends their free time {particle}. '
node = activity
code,desc,fields, img='','','',None
items={}
consumed={}

while node:
	# simple code and description
	code+=node.get('code','')
	desc+=node.get('text','')
	img=node.get('img',img)
	# items
	node_items= node.get('items',{})
	if typeof(node_items)=='str':
		node_items=[ni.strip() for ni in node_items.split(',')]
	if typeof(node_items)=='SafeList':
		node_items={ ni:node_items.count(ni) for ni in node_items }
	for (i,q) in node_items.items():
		items[i]=(f'{items[i]}+' if i in items else '')+str(q)

	# consumables
	node_ccs= node.get('consume',{})
	if typeof(node_ccs)=='str':
		node_ccs=[ncc.strip() for ncc in node_ccs.split(',')]
	if typeof(node_ccs)=='SafeList':
		node_ccs={ncc:node_ccs.count(ncc) for ncc in node_ccs}
	for (cc,q) in node_ccs.items():
		consumed[cc]=(f'{consumed[cc]}+' if cc in consumed else '')+str(q)

	# check nested table
	table=node.get('table')
	if table:
		if roll_expr:=table.get('roll'):
			r = vroll('+'.join([str(get(e)) + f"[{e[:3]}]" if e.isidentifier() else e for e in roll_expr.split('+')]))
			fields += f'-f Roll|"{r.full}"|inline '
		elif skill:=table.get('skill'):
			r = vroll( character().skills[skill].d20())
			fields += f'-f {game_data["skills"][skill]}|"{r.full}"|inline '
		elif tool := table.get('tool'):
			prof=0.5 if get('BardLevel',0)>2 else 0
			prof=1 if tool in get('pTools','') else prof
			prof=2 if get('eTools','') else prof
			prof*=proficiency
			r = vroll(f'1f20+{int(prof)}')
			fields += f'-f {tool}|"{r.full}"|inline '
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

# the items and consumables are modified in a separate gvar routine, after the {{code}} is executed, so variables declared in the downtime json
# Apply the changes to the character
char.mod_cc(ccn,-1,True)
fields+=f'-f Downtime|"{cc_str(ccn)}"|inline '
if not desc:
	desc='Nothing happens.'
update_code= get_gvar('dda143a1-2586-4d03-8fa6-3ee1e204f87b').replace('@I@',str(items)).replace('@C@', str(consumed))
return f'{code} -title "{title}" -desc "{desc}" {fields} ' + (f'-thumb {img} ' if img else '') + '{{' +update_code +'}} '
</drac2>
