tembed <drac2>
# TODO data https://www.dndbeyond.com/sources/xgte/downtime-revisited#undefined
#   Sub commnd xge to enble, phb for phb downtime nd dmg for dmg downtime stuff set in svar
# "fail" and "crit" option in table of roll result is fail or crit
# skill override !downtime perform -with drum,adv,nature|adv... < split to list of tool/skill/die, use up one per nested table until empty.
# skill override also supports + and - digit bonus option
# override also supports auto fail and crit option
# !craft <tool> or <skill> checks gamedata to prevent being confused if a tool has no recipes
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
choice=arg.split(" -",maxsplit=1)[0]
args=argparse(arg)
choices = [c for c in data.keys() if c.lower().startswith(choice)]
if arg=='?' or arg.lower()=='help' or arg=='' or not choices:
	return f'-title "{name} spends their free time getting help." -desc "`!downtime <activity>` where <activity> is one of {", ".join(data.keys())}."'
choice = choices[0]

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
activity=data[choice]
particle=activity.get('particle',choice+'ing')
downtime=char.get_cc(ccn)
if not downtime:
	return f'-title "{name} doesn\'t have time to {choice}" -desc "You have no more downtime left." -f Downtime|"{cc_str(ccn)}"|inline'

bag_var='bags'
if exists(bag_var):
	bag=load_json(get(bag_var))
else:
	bag=None

# Preconditions met, roll for results
game_data=load_json(get_gvar('c2bd6046-90aa-4a2e-844e-ee736ccbc4ab'))
title=f'{name} spends their free time {particle}. '
node = activity
code,desc,fields, img='','','',None
items={}
consumed={}
overrides=args.get('with',[])
override = overrides[0] if overrides else []
override = [o for o in override.lower().split('&') if o] if override else []
crit_node,fail_node='crit','fail'
while node:
	# simple code and description
	code+=node.get('code','')
	desc+=node.get('text','')
	img=node.get('img',img)
	# node precondition: tool ownership (not skill related, use same override as table)
	tool = node.get('tool')
	for o in override:
		tool_override = [t for t in game_data.get('tools', []) if t.lower().startswith(o)]
		if tool_override:
			tool = tool_override[0]
	if bag and tool:
		if not any([b[1].get(tool, 0) > 0 for b in bag]):
			return f'-title "{name} doesn\'t have the tools to {choice}" -desc "You need a {tool}."'

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
		override = overrides.pop(0) if overrides else None
		override = [o for o in override.lower().split('&') if o] if override else []

		# get tool with override
		# get skill with override
		tool = table.get('tool')
		skill = table.get('skill')
		for o in override:
			tool_override = [t for t in game_data.get('tools', []) if t.lower().startswith(o)]
			if tool_override:
				tool = tool_override[0]
				skill = None	# override also default skill
		if bag and tool and not any([b[1].get(tool, 0) > 0 for b in bag]):
			return f'-title "{name} doesn\'t have the tools to {choice}" -desc "You need a {tool}."'

		for o in override:
			skill_override = [s for (s, d) in game_data.get('skills', {}).items() if
							  s.lower().startswith(o) or d.lower().startswith(o)]
			if skill_override:
				skill = skill_override[0]

		# if fail or critical is oveerriden and defined, don't bother making a roll
		roll_desc = game_data["skills"][skill] if skill else tool if tool else 'Roll'
		if crit_node in table and 'crit' in override:
			node = table[crit_node]
			fields += f'-f "{roll_desc}"|crit|inline '
		elif fail_node in table and 'fail' in override:
			node = table[fail_node]
			fields += f'-f "{roll_desc}"|fail|inline '
		else:
			# make base roll string for tool and 1d20 rolls
			adv_override = True if 'adv' in override else False if 'dis' in override else None
			rollstr = {None: '1d20', True: '2d20kh1', False: '2d20kl1'}[adv_override]
			# add argument override bonuses
			boni = []
			for o in override:
				if o[0] == '+':
					boni += [o[1:]]
				elif o[0] == '-':
					boni += [o]
				elif o.isdigit():
					boni += [o]

			# jank way to force critical/fail and still roll
			if 'crit' in override:
				rollstr+='rr<20'
			elif 'fail' in override:
				rollstr+='rr>1'
			if skill:
				# TODO: skill checks don't support forced 1 or 20
				r = vroll('+'.join([character().skills[skill].d20(base_adv=adv_override)]+boni))
			elif tool:
				prof=0.5 if get('BardLevel',0)>2 else 0
				prof=1 if tool in get('pTools','') else prof
				prof=2 if get('eTools','') else prof
				prof*=proficiencyBonus
				r = vroll('+'.join([rollstr,str(int(prof))]+boni))
			elif roll_expr:=table.get('roll'):
				if roll_expr.lower().startswith('1d20'):
					roll_expr=rollstr+rollexpr[4:]
				r = vroll('+'.join([str(get(e)) + f"[{e[:3]}]" if e.isidentifier() else e for e in roll_expr.split('+')]+boni))
			else:
				r = vroll('+'.join([rollstr]+boni))
			if r:
				fields += f'-f "{roll_desc}"|"{r.full}"|inline '

			if crit_node in table and r.result.crit==1:
				node=table[crit_node]
			elif fail_node in table and r.result.crit==2:
				node=table[fail_node]
			else:
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
