<drac2>
# TODO: pre compute location of all combatants and put in dict
# show class or monster
# (persistent) arguments to make health as %, number, bar or values or quality
# option for range in feet #
# argument to make class/effects/conditions shown or hidden

# add all that to base !state for all characters in combat. (range, concentration, effects)

C=combat()
if not C:
	return f'techo 8 The current channel is not in combat.'


result=[]
roundstr=[':zero:',':one:',':two:',':three:',':four:',':five:',':six:',':seven:',':eight:',':nine:',':keycap_ten:']
header=f'Combat Round {C.round_num if C.round_num>=len(roundstr) else roundstr[C.round_num]} in "{ctx.channel.name}"'

args=argparse('''&*&''')
timeout = args.last('t','auto')
timeunit=dict(m=60,min=60,s=1,sec=1)
for unit,mul in timeunit.items():
	if timeout.lower().endswith(unit):
		timeout=timeout[:-len(unit)]
		if timeout.isdecimal():
			timeout = mul * int(timeout)
			break
		else:
			return f'echo Timeout argument `-t {timeout}{unit}` syntax error. The value must be a number.'
else:
	if timeout=='auto':
		pass
	elif timeout.startwith('no'):
		timeout=None
	elif timeout.isdecimal():
		timeout = int(timeout)
		if not timeout:
			timeout=None
	else:
		return f'echo Timeout argument `-t {timeout}` syntax error. The value must be a number.'

alphabet='abcdefghijklmnopqrstuvwxyz'
x_table=[' ']+[c for c in alphabet]+[f'a{c}' for c in alphabet]+[f'b{c}' for c in alphabet]
loc_prefix='Location: '
location={}
dm_combatants=['map','dm','lair']
dm_id=None
for c in C.combatants:
	if full_note:=c.note:
		for loc_str in [sn.strip()[len(loc_prefix):].lower() for sn in full_note.split('|') if loc_prefix in sn]:
			if x_loc:=([None]+[i+1 for i, xstr in enumerate(x_table) if loc_str.startswith(xstr)])[-1]:
				location[c.name]=dict(x=x_loc,y=int(loc_str[len(x_table[x_loc-1]):]))
	if c.name.lower() in dm_combatants:
		dm_id=c.controller

ranges=dict()
if C.me:
	game_data = load_json(get_gvar('c2bd6046-90aa-4a2e-844e-ee736ccbc4ab'))
	weapons = game_data.get('weapons',{})
	if attacks:={wn:wp for wn,wp in weapons.items() if any(wn.lower() in a.name.lower() for a in C.me.attacks)}:

		reach = max(wp.get('reach',0) for wp in attacks.values())
		normal_range = max(wp.get('range',0) for wp in attacks.values())
		max_range = max(wp.get('range',0)*(4 if wp.get('ammo') else 3) for wp in attacks.values())
		ranges=dict(reach=reach*1.5, range=normal_range,max_range=max_range)

	spell_list=load_json(get_gvar('13dc3e0a-a230-40ca-8fb3-a39846300b18'))
	if spells:={sn:sp for sn,sp in spell_list.items() if sn in C.me.spellbook}:
		if cantrip_ranges:=[s.get('range',0) for s in spells.values() if s.level==0 and (s.get('save') or s.get('attack'))]:
			ranges['cantrip']=max(cantrip_ranges)
		if spell_ranges:=[s.get('range',0) for s in spells.values() if s.level>0 and (s.get('save') or s.get('attack'))]:
			ranges['spell']=max(spell_ranges)

config=load_json(get_gvar('f9fd35a8-1c8e-477c-b66e-2eeee09a4735'))
conditions=config.conditions

dbg=''
heartstr = [':skull:', ':broken_heart:', ':hearts:', ':heartbeat:', ':heartpulse:']
distancestr=['', ':dart:', ':bow_and_arrow:']
for c in C.combatants:
	if c.name in dm_combatants:
		continue
	active=C.current and c.name==C.current.name
	props=[]
	if c.max_hp:
		health=(c.hp>0) + (c.hp>=c.max_hp/7) + (c.hp>=c.max_hp/2) + (c.hp>=c.max_hp)
		props.append(f'{heartstr[health]}{":heavy_plus_sign:" if c.temp_hp>0 else ""}')

	for e in c.effects:
		if prop:=[c_str for cond,c_str in conditions.items() if cond in e.name.lower()]:
			props.append(''.join(prop))
		elif e.conc and e.children:
			props.append(':brain:')
		else:
			props.append(e.name)

	desc='npc' if c.levels.get('Monster')>0.0 else c.levels	# TODO: option

	you,distance='',''
	if C.me and C.me.name==c.name:
		you='(You)'
	else:
		you='(Yours)' if c.controller == ctx.author.id else f'({c.group})' if c.group else 'DM' if c.controller==dm_id else ''
		if C.me and (me_loc:=location.get(C.me.name)) and (c_loc:=location.get(c.name)):
			dx=me_loc.x-c_loc.x
			dy=me_loc.y-c_loc.y
			d=sqrt(dx*dx + dy*dy)*5.0
			distance = ':crossed_swords:' if d<=ranges.reach else distancestr[(d<=ranges.range) + (d<=ranges.max_range)]
			distance += ':magic_wand:' if d<ranges.get('cantrip',-1) else ''
			distance += ':sparkles:' if d<ranges.get('spell',-1) else ''
	result.append(f'{":arrow_forward:" if active else ":white_large_square:"} **{c.name}** {you} {distance}{" - " if distance and props else ""}{", ".join(props)}')

# limit result size to 4096 discord embed description limit, discord also cuts the field short with too many smileys
# and the whole embed can only be 6000
total_limit, txt_limit, smile_limit, results=5900-len(header), 4000, 150, [[]]
for item in result:
	# item+=f'{item.count(":")} {150-smile_limit}'
	total_limit-=len(item)+1
	if total_limit<0:
		break
	txt_limit-=len(item)+1
	smile_limit-=item.count(':')//2
	if txt_limit<=0 or smile_limit<=0:
		if len(results)>=24:
			results.append(['*(more)*'])
			break
		else:
			results.append([])
			txt_limit=1000-len(item)
			smile_limit=150-item.count(':')//2
	results[-1].append(item)

nl='\n'
help_str=f'{ctx.prefix}help {ctx.alias} combat for legend.'
if timeout=='auto':
	timeout=45+len(result)*5
time_str=f'-footer "{help_str}"' if timeout is None else f'-t {timeout} -footer "This information will disappear in {f"{timeout} seconds" if timeout<120 else f"{timeout//60} minutes"}. {help_str}"'
desc=nl.join(results[0])
fields=nl.join(f'-f "{nl.join(f)}"' for f in results[1:])
return f'''embed -title "{header}" -desc "{desc}" {fields} {time_str} {dbg}'''
</drac2>
