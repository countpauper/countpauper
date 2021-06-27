<drac2>
# TODO: pre compute location of all combatants and put in dict
# compute melee/ranged/extended range of all attacks
# compute range of all attack type cantrips/ (use magic list, add range?)
# compute range of all spells
# show range: melee is swords, ranged is bow, extended range is bow with ? or telescope out of range is nothing/telescope, cantrip is wand, spell is sparkles
#
# show concentration as icon
# show condition effects with icons (unconscious, restrained, grappled, poisoned and so on)
# SHow DM controlled by getting controller id of map/dm
# show class or monster
#
# add all that to base !state for all characters in combat. (range, concentration, effects)

C=combat()
if not C:
	return f'echo The current channel is not in combat.'
result=[]
roundstr=[':zero:',':one:',':two:',':three:',':four:',':five:',':six:',':seven:',':eight:',':nine:',':keycap_ten:']
header=f'**Combat "{ctx.channel.name}"** Round: {C.round_num if C.round_num>=len(roundstr) else roundstr[C.round_num]}'

alphabet='abcdefghijklmnopqrstuvwxyz'
x_table=[' ']+[c for c in alphabet]+[f'a{c}' for c in alphabet]+[f'b{c}' for c in alphabet]
loc_prefix='Location: '
location={}
for c in C.combatants:
	if full_note:=c.note:
		for loc_str in [sn.strip()[len(loc_prefix):].lower() for sn in full_note.split('|') if loc_prefix in sn]:
			if x_loc:=([i+1 for i,xstr in enumerate(x_table) if loc_str.startswith(xstr)]+[None])[0]:
				location[c.name]=dict(x=x_loc,y=int(loc_str[len(x_table[x_loc]):]))

ranges=dict()
if C.me:
	game_data = load_json(get_gvar('c2bd6046-90aa-4a2e-844e-ee736ccbc4ab'))
	weapons = game_data.get('weapons',{})
	attacks={wn:wp for  wn,wp in weapons if any(wn in a.name for a in C.me.attacks)}

	reach = max(wp.get('reach') for wp in attacks.values())
	range = max(wp.get('range') for wp in attacks.values())
	max_range = max(wp.get('range')*(4 if w.get('ammo') else 3) for wp in attacks.values())
	ranges=dict(reach=reach, range=range,max_range=max_range)
	return f'echo {attacks} {ranges}'

ignored_combatants=['map','dm']
heartstr = [':skull: Dying', ':broken_heart: Critical', ':hearts: Bloodied', ':heartbeat: Hurt', ':heartpulse: Healthy']
distancestr=[':crossed_swords:', ':bow_and_arrow:', ':telescope:']
for c in C.combatants:
	if c in ignored_combatants:
		continue
	active=c.name==C.current.name
	props=[]
	if c.max_hp:
		health=(c.hp>0) + (c.hp>=c.max_hp/7) + (c.hp>=c.max_hp/2) + (c.hp>=c.max_hp)
		props+=[heartstr[health]]
	props+=[e.name for e in c.effects]
	you,distance='',''
	if C.me and C.me.name==c.name:
		you='(You)'
	else:
		you='(Yours)' if c.controller == ctx.author.id else f'({c.group.name})' if c.group else ''
		if C.me and (me_loc:=location.get(C.me.name)) and (c_loc:=location.get(c.name)):
			dx=me_loc.x-c_loc.x
			dy=me_loc.y-c_loc.y
			d=sqrt(dx*dx + dy*dy)*5.0
			distance=(d<8)+(d<=90)
			distance=distancestr[distance]
	result.append(f'{":arrow_forward:" if active else ":white_large_square:"} **{c.name}** {you} {distance} {", ".join(props)}')
nl='\n'
return f'echo {header}\n{nl.join(result)}'
</drac2>
