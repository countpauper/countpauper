tembed <drac2>
# TODO
# CC find in character consumables to be case insensitive?
# apply effect (but only in combat and this is !downtime, sooo)
# neutral resistances and 'applies_to'
# "Inspiration" (just a cc?) snippet & cc, inspiration as a result option (NB inspiration effect and snippet used for bard)
# relation added to !relation, need a table for location?(channel)

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
if cc_max is None or cc_max>0:
	char.create_cc(ccn, minVal=0, maxVal=cc_max, reset=cc_reset if cc_max else None, dispType='bubble' if cc_max and cc_max<=5 else None)
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
modifiable=['hp','thp']	# TODO: xp, gp?
modified={}
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
		# partial override match with tool, but must be whole word or 'dis' matches with 'disguise kit'
		tool_override = [t for t in game_data.get('tools', []) if t.lower()==o or o==t.lower().replace("'",' ').split(' ')[0]]
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
	if extra_item:= node.get('item'):
		items[exta_item] = items.get(extra_item,0)+1

	# consumables
	node_ccs= node.get('consume',{})
	if typeof(node_ccs)=='str':
		node_ccs=[ncc.strip() for ncc in node_ccs.split(',')]
	if typeof(node_ccs)=='SafeList':
		node_ccs={ncc:node_ccs.count(ncc) for ncc in node_ccs}
	for (cc,q) in node_ccs.items():
		consumed[cc]=(f'{consumed[cc]}+' if cc in consumed else '')+str(q)

	# character modifications
	for mod in modifiable:
		if modification:=node.get(mod):
			modified[mod]=f'{modified[mod]}+{modification}' if mod in modified else str(modification)

	# check nested table
	table=node.get('table')
	if table:
		tool = table.get('tool')
		skill = table.get('skill')
		# get -with and all -<skill> overrides
		override = overrides.pop(0) if overrides else ''
		if skill:
			override = '&'.join([override]+args.get(skill,[]))
		override = [o for o in override.lower().split('&') if o] if override else []

		# get tool with override
		for o in override:
			# partial override match with tool, but must be whole word or 'dis' matches with 'disguise kit'
			tool_override = [t for t in game_data.get('tools', []) if t.lower()==o or t.lower()==o.replace("'",' ').split(' ')[0]]
			if tool_override:
				tool = tool_override[0]
				skill = None	# override also default skill
		if bag and tool and not any([b[1].get(tool, 0) > 0 for b in bag]):
			return f'-title "{name} doesn\'t have the tools to {choice}" -desc "You need a {tool}."'

		# get skill with override
		for o in override:
			skill_override = [s for (s, d) in game_data.get('skills', {}).items() if
							  s.lower().startswith(o) or d.lower().startswith(o)]
			if skill_override:
				skill = skill_override[0]

		# if fail or critical is overriden and defined, don't bother making a roll
		roll_desc = tool if tool else game_data["skills"][skill] if skill else 'Roll'
		if crit_node in table and 'crit' in override:
			next = table[crit_node]
			fields += f'-f "{roll_desc}"|Critical|inline '
		elif fail_node in table and 'fail' in override:
			next = table[fail_node]
			fields += f'-f "{roll_desc}"|Fail|inline '
		else:
			# make base roll string for tool and 1d20 rolls
			rolladv=0
			rollstr='1d20'
			prof = ''
			boni = []
			s=None
			# add tool proficiency to boni
			if tool:
				prof = f'{proficiencyBonus//2}[jack]' if get('BardLevel', 0) > 2 else prof
				prof = f'{proficiencyBonus}[{tool}]' if tool in get('pTools', '') else prof
				prof = f'{2*proficiencyBonus}[{tool}]' if tool in get('eTools','') else prof
			# add skill bonus
			if skill:
				s = char.skills[skill]
				sname = game_data['skills'][skill]
				if s.prof*proficiencyBonus>=roll(prof):
					boni+=[f'{s.value}[{sname}]']
				else:	# use tool prof, deduct skill prof (eg jack on ability check)
					boni+=[f'{s.value-int(s.prof*proficiencyBonus)}[{sname}]']
				rolladv +=1 if s.adv is True else -1 if s.adv is False else 0
			elif rollexpr:=table.get('roll'):
				roll_split = rollexpr.replace('//',' ').translate("".maketrans("+-*/", "    ", " \t\n")).split()
				rollstr=roll_split[0]
				rollexpr=rollexpr[len(rollstr)+1:].strip()
				if rollexpr:
					for e in roll_split[1:]:
						if e.isidentifier():
							rollexpr = rollexpr.replace(e,f'{get(e)}[{e[:3]}]')
					boni+=[rollexpr]
			# apply skill or tool proficiency bonus
			if prof:
				boni = [prof] + boni
			# apply overrides
			for o in override:
				if o[0] == '+':
					boni += [o[1:]]
				elif o[0] == '-':
					boni += [o]
				elif o.isdigit():
					rollstr=o
				elif o == 'fail':
					rollstr = '1'
				elif o == 'crit':
					rollstr = '20'
				elif o=='adv':
					rolladv+=1
				elif o=='dis':
					rolladv-=1
			# Reliable talent
			if rollstr[1:4]=='d20' and (roll(prof)>=proficiencyBonus or (s and s.prof>=1)) and char.csettings.get('talent',False):
				rollstr = rollstr[:4]+'mi10'+rollstr[4:]
			# halfing luck
			elif rollstr[1:4]=='d20' and (reroll:=char.csettings.get('reroll',0)):
				rollstr=rollstr[:4]+f'ro{reroll}'+rollstr[4:]

			# apply the counted advantage of all overrides and skill
			if rollstr.startswith('1d'):
				if rolladv>0:
					rollstr='2d'+rollstr[2:]+'kh1'
				elif rolladv<0:
					rollstr='2d'+rollstr[2:]+'kl1'
			rollstr='+'.join([rollstr]+boni)
			r = vroll(rollstr)
			fields += f'-f "{roll_desc}"|"{r.full}"|inline '

			if crit_node in table and r.result.crit==1:
				next=table[crit_node]
			elif fail_node in table and r.result.crit==2:
				next=table[fail_node]
			else:
				options=[int(k) for k in table.keys() if (k.isdigit() or (k[0]=='-' and k[1:].isdigit())) and int(k) <= r.total]
				if options:
					next=table[str(max(options))]
				else:
					next=table.get('default',None)
	else:
		next=None
	# quick leaf node with only text is added to description
	if typeof(next)=='str':
		desc+=next
		node=None
	else:
		node=next

# the items and consumables are modified in a separate gvar routine, after the {{code}} is executed, so variables declared in the downtime json
# Apply the changes to the character
char.mod_cc(ccn,-1,True)
fields+=f'-f Downtime|"{cc_str(ccn)}"|inline '
if not desc:
	desc='Nothing happens.'
update_code= get_gvar('dda143a1-2586-4d03-8fa6-3ee1e204f87b').replace('@I@',str(items)).replace('@C@', str(consumed)).replace('@M@',str(modified))
return f'{code} -title "{title}" -desc "{desc}" {fields} ' + (f'-thumb {img} ' if img else '') + '{{' +update_code +'}} '
</drac2>
