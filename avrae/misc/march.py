<drac2>
C=combat()
if not C:
	return f'echo You can only `{ctx.prefix}{ctx.alias}` in combat.'
args = argparse(&ARGS&)

# TODO: read out diagonal configuration. default is Euclidian. setting is "diagonal":ft. use 10 for manhattan or 5, 7.07, 7.5
# when computing distance this is basically just two variations. diagonal multiplied for min(abs(dx),abs(dy))+straight for abs(abs(dx)-abs(dy)) or sqrt(dx*dx+dy*dy)

# some constants
loc_prefix = 'Location: '
size_prefix = 'Size: '
default_size=1
sizes = dict(L=2, H=3, G=4)
ft_per_grid=5
alphabet='abcdefghijklmnopqrstuvwxyz'
x_axis={col:idx+1 for idx,col in enumerate(alphabet)}
for mul,prefix in enumerate('abc'):
	x_axis.update({f'{prefix}{col}':(1+mul)*len(alphabet)+idx+1 for idx,col in enumerate(alphabet)})
default_speed=float(args.last('ft',1000))	 # TODO context

# inclusive map bounds
map_rect=(1,1,len(x_axis),100)	# TODO get from combat map. it's in its attack description's text field, which is part of the raw.automation[0]
# combat().get_combatant('map').attacks[0].raw.automation[0].text.split('~')  (attack's name is also 'map')

### Parse target arguments into a dict of mover_name:dict(c=combatant,s=speed)
if targets:=args.get('t'):
	# split the target aruguments into target id and config (only speed for now)
	speeds = {split_t[0]: int(split_t[1]) for split_t in [t.split('|') + [default_speed] for t in targets]}
	# find all targeted combatants or groups
	combatants = {t: C.get_combatant(t) or C.get_group(t) for t in speeds.keys() if typeof(t)=='str'}
	# TODO: error? not_found = [t for t, c in combatants.items() if not c]
	movers = dict()

	# TODO: override duplicates by storing movers by name and adding the c as a dict
	for t,c in combatants.items():
		if not c:
			continue
		if c.type == 'group':
			movers.update({member.name:dict(c=member, s=speeds.get(t,default_speed)) for member in c.combatants})
		else:
			movers[c.name]=dict(c=c,s=speeds.get(t,default_speed))
elif not C.current:
	return f'echo There is no current combatant, use `{ctx.prefix}{ctx.alias} -t <group>`'
elif C.current.type=='group':
	movers={c.name:dict(c=c, s=default_speed) for c in C.current.combatants}
else:
	movers={C.current.name:dict(c=C.current,s=default_speed)}

### create a dict with x,y,s (exclusive) squares of combatants
# and a list of non moving combatants with a location that block targets or goals
locations=dict()
blockers=dict()
for c in C.combatants:
	notes=[n.strip() for n in c.note.split('|') if n] if c.note else []
	x,y,s=None,None,default_size
	for n in notes:
		if n.startswith(loc_prefix):
			location=n[len(loc_prefix):].lower()
			x_loc_str=''.join(c for c in location if c.isalpha())
			x=x_axis.get(x_loc_str)
			y_loc_str=location[len(x_loc_str):]
			y=int(y_loc_str) if y_loc_str.isdecimal() else None
		elif n.startswith(size_prefix):
			s=sizes.get(n[len(size_prefix)],s)
	if x is not None and y is not None:
		locations[c.name] = dict(x=x, y=y, s=s)
		if not any(mover_name==c.name for mover_name in movers.keys()):
			blockers[c.name]=locations[c.name]

# convert the movers list to a dictionary per mover with all necessary data
movers=[dict(c=m.c,l=locations.get(n),size=default_size,speed=m.s/ft_per_grid,goal=None,target=None) for n,m in movers.items()]

# TODO: l[ine] and r[ectangle] targets first so their spread can be evened out to len(movers)
# TODO: first parse all formation arguments into a [shape,area] list so they can all use the same coordinate parsing
# TODO: approach targets can spiral out for number of movers, but should go to first row not closest when assigning so ... not easy
targets=[]
for area in args.get('approach')+args.get('a'):
	parts=[p.strip().lower() for p in area.split(':')][:2]
	area_rect=tuple()	 # Left,Top,Right,Bottom
	if len(parts)==1:
		if matches:=[c for c in locations.keys() if c.lower().startswith(parts[0])]:
			loc=locations[matches[0]]
			area_rect=(loc.x, loc.y, loc.x+loc.s-1, loc.y+loc.s-1)
		else:
			parts.append(parts[0])
	if not area_rect:
		for p_str in parts:
			x_loc_str = ''.join(c for c in p_str if c.isalpha())
			x_loc = x_axis.get(x_loc_str)
			y_loc_str = p_str[len(x_loc_str):]
			y_loc = int(y_loc_str) if y_loc_str.isdecimal() else None
			if x_loc is None or y_loc is None:
				err(f'Approach target `{area}` is malformed')
			area_rect+=(x_loc,y_loc)

		# sort top left top left
		area_rect=(min(area_rect[0::2]),min(area_rect[1::2]),max(area_rect[0::2]),max(area_rect[1::2]))

	# plot target locations
	for x in range(area_rect[0]-1,area_rect[2]+1):
		targets.append((x,area_rect[1]-1))
		targets.append((x+1,area_rect[3]+1))
	for y in range(area_rect[1]-1,area_rect[3]+1):
		targets.append((area_rect[0]-1, y+1))
		targets.append((area_rect[2]+1,y))

# clip targets to map area and remove targets overlapping with blockers
unclipped_targets,targets=targets,[]
size=1	 # TODO set to maximum size +1
for t in unclipped_targets:
	# clip to map
	if t[0]<map_rect[0] or t[1]<map_rect[1] or t[0]>map_rect[2] or t[1]>map_rect[3]:
		continue
	# remove duplicates
	if t in targets:
		continue
	# remove targets overlapping with existing blockers
	if any(t[0]+s>b.x and t[1]+s>b.y and t[0]<b.x+b.s and t[1]<b.y+b.s for b in blockers.values()):
		continue

	targets.append(t)

# return f'echo {unclipped_targets} in {map_rect} and not in {blockers} => {targets}'

# Assign movers to targets, in order of movers, sorted preferred target by distance
for m in movers:
	if not m.target:
		if not m.l and targets:	# place
			m['goal']=targets.pop(0)
			m['target']=m.goal
		else:
			best_idx, best_dist=None, 0
			for idx,t in enumerate(targets):
				dx=m.l.x - t[0]
				dy=m.l.y - t[1]
				sqr_d=dx*dx + dy*dy	# TODO different speed diagonals
				if not best_idx or sqr_d<best_dist:
					best_dist=sqr_d
					best_idx=idx
			if best_idx is not None:
				m['goal']=targets.pop(best_idx)

# move towards target with speed
for m in movers:
	if not m.goal or m.target:
		continue
	dx,dy=m.goal[0] - m.l.x,m.goal[1]-m.l.y
	vector_length = sqrt(dx*dx+dy*dy)
	if vector_length<=m.speed:
		m['target']=m.goal
		continue
	# find maximum movement vector
	normalize_multiplier=(m.speed)/vector_length
	dx*=normalize_multiplier
	dy*=normalize_multiplier
	# TODO: find existing targets and avoid by finding other options IN RANGE.
	# TODO: also avoid non moving enemies (put those in the list first, add targets)
	# TODO: Take size into account
	# TODO: what to do with movers that are not blockers but then they can't move so now they were
	m['target']=round(m.l.x+dx), round(m.l.y+dy)

inv_x_axis={val:s.upper() for s,val in x_axis.items()}
description='\n'.join(f'{m.c.name} @ ({m.l}) :black_square_button:  {m.size*ft_per_grid}ft. :fast_forward: {m.speed*ft_per_grid}ft. :goal: {m.goal} :person_walking:  {m.target}' for m in movers)
space,quote=' ','\\"'
cmd_args=[f'-t {quote if space in m.c.name else ""}{m.c.name}|{inv_x_axis[m.target[0]]}{m.target[1]}{quote if space in m.c.name else ""}' for m in movers if m.target]
cmd_field=f'-f "Command|`!map {space.join(cmd_args)}`"' if cmd_args else ''

return f'embed -title March! -desc "{description}" {cmd_field}'
</drac2>