<drac2>
# TODO: pre compute location of all combatants and put in dict
# show class or monster
# (persistent) arguments to make health as %, number, bar or values or quality
# option for timeout in seconds
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
timeout = args.last('t',"60")

alphabet='abcdefghijklmnopqrstuvwxyz'
x_table=[' ']+[c for c in alphabet]+[f'a{c}' for c in alphabet]+[f'b{c}' for c in alphabet]
loc_prefix='Location: '
location={}
dm_combatants=['map','dm']
dm_id=None
for c in C.combatants:
	if full_note:=c.note:
		for loc_str in [sn.strip()[len(loc_prefix):].lower() for sn in full_note.split('|') if loc_prefix in sn]:
			if x_loc:=([i+1 for i,xstr in enumerate(x_table) if loc_str.startswith(xstr)]+[None])[0]:
				location[c.name]=dict(x=x_loc,y=int(loc_str[len(x_table[x_loc]):]))
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
	result.append(f'{":arrow_forward:" if active else ":white_large_square:"} **{c.name}** {you} {distance} {", ".join(props)}')
nl='\n'
help_str=f'{ctx.prefix}help {ctx.alias} combat for legend.'
timeout=int(timeout) if timeout.isdecimal() else None
time_str=f'-footer "{help_str}"' if timeout is None else f'-t {timeout} -footer "This information will disappear in {f"{timeout} seconds" if timeout<120 else f"{timeout//60} minutes"}. {help_str}"'
return f'embed -title "{header}" -desc "{nl.join(result)}" {time_str}'
</drac2>
