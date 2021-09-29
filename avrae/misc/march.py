<drac2>
C=combat()
if not C:
	return f'echo You can only `{ctx.prefix}{ctx.alias}` in combat.'
args = argparse(&ARGS&)

config=load_json(get('march','{}'))
# TODO: read out diagonal configuration. default is Euclidian. setting is "diagonal":ft. use 10 for manhattan or 5, 7.07, 7.5
# when computing distance this is basically just two variations. diagonal multiplied for min(abs(dx),abs(dy))+straight for abs(abs(dx)-abs(dy)) or sqrt(dx*dx+dy*dy)

# some constants

sizes = dict(L=2, H=3, G=4)
ft_per_grid=5
size_arg=args.last('size','').upper()
default_size=sizes.get(size_arg[:1], int(size_arg)//ft_per_grid if size_arg.isdecimal() else 1)
alphabet='abcdefghijklmnopqrstuvwxyz'
x_axis={col:idx+1 for idx,col in enumerate(alphabet)}
for mul,prefix in enumerate('abc'):
	x_axis.update({f'{prefix}{col}':(1+mul)*len(alphabet)+idx+1 for idx,col in enumerate(alphabet)})
default_speed=float(args.last('ft',30))

# inclusive map bounds
map_rect=dict(x=1,y=1,w=len(x_axis),h=100)	# TODO get from combat map. it's in its attack description's text field, which is part of the raw.automation[0]
map_combatants=[C.get_combatant('map'),C.get_combatant('dm'),C.get_combatant('lair')]
map_attacks=[a.raw.automation for c in map_combatants if c is not None for a in c.attacks if a.name=='map']
if map_attacks and (map_automation:=map_attacks[0]):
	size_prefix = 'Size: '
	if size_property:=([prop for prop in map_automation[0].get('text').split(' ~ ') if prop.startswith(size_prefix)]+[None])[0]:
		map_rect['w'],map_rect['h']=[int(sp) for sp in size_property[len(size_prefix):].split('x') if sp.isdecimal()]

not_found=[]
### Parse target arguments into a dict of mover_name:dict(c=combatant,s=speed)
if targets:=args.get('t'):
	# split the target arguments into target id and config (only speed for now)
	# TODO: could also add size per each target -t <target>|speed|size and add size to map command if changed or new
	speeds = {split_t[0]: int(split_t[1]) for split_t in [t.split('|') + [default_speed] for t in targets]}
	# find all targeted combatants or groups
	combatants = {t: C.get_combatant(t) or C.get_group(t) for t in speeds.keys() if typeof(t)=='str'}
	movers = dict()

	# TODO: override duplicates by storing movers by name and adding the c as a dict
	for t,c in combatants.items():
		if not c:
			not_found.append(t)
		elif c.type == 'group':
			movers.update({member.name:dict(combatant=member, speed=speeds.get(t,default_speed)) for member in c.combatants})
		else:
			movers[c.name]=dict(combatant=c,speed=speeds.get(t,default_speed))
elif not C.current:
	return f'echo There is no current combatant, use `{ctx.prefix}{ctx.alias} -t <group>`'
elif C.current.type=='group':
	movers={c.name:dict(combatant=c, speed=default_speed) for c in C.current.combatants}
else:
	movers={C.current.name:dict(combatant=C.current,speed=default_speed)}

if not movers:
	return f'echo The following targets were not found: `{", ".join(not_found)}`'

### create a dict with x,y,s (exclusive) squares of combatants
# and a list of non moving combatants with a location that block targets or goals
locations=dict()
blockers=dict()
loc_prefix = 'Location: '
size_prefix = 'Size: '
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

# convert the movers list to a dictionary per mover with all necessary data
movers={n:dict(combatant=m.combatant,
			   start=locations.get(n),
			   size=locations.get(n,dict(s=default_size)).s,
			   speed=m.speed/ft_per_grid,
			   goal=None,
			   target=None,
			   block=None,
			   dbg=[]) for n,m in movers.items()}

# add all non moving map characters to the list of blockers
for name,loc in locations.items():
	if not name in movers.keys():
		blockers[name] = dict(x=loc.x, y=loc.y, w=loc.s, h=loc.s)

### Parse all goal arguments (approach, line, rect and the block argument)
# the result is a dictionary {type: [dict(x,y,w,h),...]}
# these are all done together to avoid code duplication for the different types
# this means they all support the format A1 or A1:C3 or go1 (even when it makes no sense)
zone_args=dict(approach=args.get('approach')+args.get('a'),
		   line=args.get('line')+args.get('l'),
		   rect=args.get('rect')+args.get('r'),
		   block=args.get('block')+args.get('b'))
zones={z:[] for z in zone_args.keys()}
for zone_type, zone_args in zone_args.items():
	for area in zone_args:
		# TODO: can extra split by comma here or before for -block a1,b2,c3:d4 or -approach a1,f4
		parts = [p.strip().lower() for p in area.split(':')][:2]
		area_rect=dict()  # Left,Top,width, height
		if len(parts)==1:
			if matches:=[c for c in locations.keys() if c.lower().startswith(parts[0])]:
				loc=locations[matches[0]]
				area_rect=dict(x=loc.x, y=loc.y, w=loc.s, h=loc.s)
			else:
				parts.append(parts[0])
		if not area_rect:
			area_coordinates=tuple()
			for p_str in parts:
				x_loc_str = ''.join(c for c in p_str if c.isalpha())
				x_loc = x_axis.get(x_loc_str)
				y_loc_str = p_str[len(x_loc_str):]
				y_loc = int(y_loc_str) if y_loc_str.isdecimal() else None
				if x_loc is None or y_loc is None:
					err(f'{zone_type} area `{area}` is malformed')
				area_coordinates+=(x_loc,y_loc)

			# sort top left top left
			area_coordinates=(min(area_coordinates[0::2]),min(area_coordinates[1::2]),max(area_coordinates[0::2]),max(area_coordinates[1::2]))
			area_rect=dict(x=area_coordinates[0],
						   y=area_coordinates[1],
						   w=1+area_coordinates[2]-area_coordinates[0],
						   h=1+area_coordinates[3]-area_coordinates[1])
		zones[zone_type].append(area_rect)


for idx,block in enumerate(zones.block):
	blockers[f'Block#{idx}'] = block

### Construct goal locations into a list [(x,y),...]
# TODO: l[ine] and r[ectangle] targets first so their spread can be evened out to len(movers)
goals=[]
for area in zones.rect:
	for y in range(area.y, area.y + area.h):
		# serpentine for nicer distribution
		if y%1:
			for x in range(area.x+area.w,area.x,-1):
				goals.append((x-1,y))
		else:
			for x in range(area.x,area.x+area.w):
				goals.append((x,y))

for area in zones.line:
	if area.w==0 and area.h==0:
		goals.append((area.x,area.y))
	else:
		if area.w>area.h:
			if area.w==1:
				dx,dy,q=0,0,1
			else:
				dx,dy,q=1,(area.h-1)/(area.w-1),area.w
		else:
			if area.h==1:
				dx,dy,q=0,0,1
			else:
				dx,dy,q=(area.w-1)/(area.h-1), 1,area.h
		goals+=[(round(area.x+idx*dx),round(area.y+idx*dy)) for idx in range(q)]

for area in zones.approach:
	# plot target locations
	for x in range(area.x-1,area.x+area.w):
		goals.append((x,area.y-1))
		goals.append((x+1,area.y+area.h))
	for y in range(area.y-1,area.y+area.h):
		goals.append((area.x-1, y+1))
		goals.append((area.x+area.w,y))


### clip goals to map area and remove duplicate goals or those overlapping with blockers
unclipped_goals,goals=goals,[]
for goal in unclipped_goals:
	# clip to map
	if goal[0]<map_rect.x or goal[1]<map_rect.y or goal[0]>=map_rect.x+map_rect.w or goal[1]>=map_rect.y+map_rect.h:
		continue
	# remove duplicates
	if goal in goals:
		continue
	# remove goals overlapping with existing blockers
	if any(goal[0]+s>b.x and goal[1]+s>b.y and goal[0]<b.x+b.w and goal[1]<b.y+b.h for b in blockers.values()):
		continue
	# it's good
	goals.append(goal)

### thin or scatter out for the number of movers
# todo: scatter algorithm is: shuffle before thinning
if args.last('scatter'):
	ordered_goals,goals=goals,[]
	while ordered_goals:
		goals.append(ordered_goals.pop(randint(len(ordered_goals))))

if (density:=len(goals)//len(movers))>1:
	goals=goals[::density]


#return f'echo {unclipped_goals} in {map_rect} and not in {blockers} => {goals}'

### Assign movers to goals, in order of movers, sorted preferred target by distance
for mover in movers.values():
	if not mover.target:
		if not mover.start and goals:	# place
			mover['goal']=goals.pop(0)
			mover['target']=mover.goal
		else:
			best_idx, best_dist=None, 0
			for idx,t in enumerate(goals):
				dx=mover.start.x - t[0]
				dy=mover.start.y - t[1]
				sqr_d=dx*dx + dy*dy	# TODO different speed diagonals
				if not best_idx or sqr_d<best_dist:
					best_dist=sqr_d
					best_idx=idx
			if best_idx is not None:
				mover['goal']=goals.pop(best_idx)
# convert go targets to path strings
path_str=''.join(args.get('go')).upper()
idx,go_path=0,[]
directions={'NE':(1,-1),'SE':(1,1),'SW':(-1,1),'NW':(-1,-1),'N':(0,-1),'E':(1,0),'S':(0,1),'W':(-1,0)}
while path_str:
	if number_idx:=min(idx for idx,c in enumerate(path_str+'!') if not c.isdecimal()):
		number=round(int(path_str[:number_idx])/ft_per_grid)
		path_str = path_str[number_idx:]
	else:
		number=1
	if match_dir:=([d for d in directions.keys() if path_str.startswith(d)]+[''])[0]:
		path_str=path_str[len(match_dir):]
		go_path+=[(directions[match_dir][0], directions[match_dir][1])]*number
	else:
		return f'echo Unrecognized direction `{path_str}` in `-go` '

## quick check if -go is before or after all target areas. this doesn't support both or interleaving
pre_path = go_path
## TODO: post_path use case? Line up and advance? will get whack
# post_path = []
#if go_path:
#	arg_str='''&*&'''
#	if area_args:=[arg_str.find(area_arg) for area_arg in ['-a','-l','-r']]:
#		if arg_str.find('-go')>max(area_args):
#			post_path=go_path
#			pre_path=[]
#
#return f'echo {pre_path} area {post_path}'

# move towards target with speed
for name, mover in movers.items():
	if mover.target:
		continue

	# render pre path, track movement remaining, reverse order
	path = [(mover.start.x, mover.start.y)]
	movement=mover.speed
	if pre_path:
		for dx,dy in pre_path:
			if movement<0.5:
				break
			path.insert(0,(path[0][0] + dx, path[0][1] + dy))
			# todo: diagonal distance
			movement-=sqrt(dx*dx+dy*dy)
			mover['goal']=path[0]

	# add approach path
	if mover.goal:
		pos=path[0]
		# compute normalized direction vector
		dx,dy=mover.goal[0] - pos[0], mover.goal[1]-pos[1]
		vector_length = sqrt(dx*dx+dy*dy)
		if vector_length>0.1:
			dx/=vector_length
			dy/=vector_length
			path_length=min(movement,vector_length)
			# render all grids from end to pos the path, path may contain duplicates due to rounding but that's only a small performance problem
			path=[(round(pos[0]+dx*step),round(pos[1]+dy*step)) for step in range(round(path_length),0,-1)]+path
	# TODO render post path, is there really a use case?

	# mover['dbg'].append(path)
	# find furthest non-blocked point along the path
	# mover.dbg.append(f'path {path}')
	for p in path:
		# mover.dbg.append(f'blocked at {p} by { {f"{n}({b.x},{b.y},{b.w},{b.h})":p[0] + mover.size > b.x and p[1] + mover.size > b.y and p[0] < b.x + b.w and p[1] < b.y + b.h for n,b in blockers.items()} }')
		if p[0] < map_rect.x or p[1] < map_rect.y or p[0]+mover.size > map_rect.x + map_rect.w or p[1]+mover.size > map_rect.y + map_rect.h:
			mover['block'] = 'map'
			continue
		for blocker, block in blockers.items():
			if p[0] + mover.size > block.x and p[1] + mover.size > block.y and p[0] < block.x + block.w and p[1] < block.y + block.h:
				mover['block'] = blocker
				break
		else:	# not blocked, move to p
			mover['target'] = p
			blockers[name]=dict(x=mover.target[0], y=mover.target[1], w=mover.size, h=mover.size)
			break
	else:	# end of path, stay in position
		mover['target'] = None
		blockers[name] = dict(x=mover.start.x, y=mover.start.y, w=mover.size, h=mover.size)

### generate embed text
inv_x_axis={val:s.upper() for s,val in x_axis.items()}
space,nl,quote=' ','\n','"'
descriptions=[]
for name, mover in movers.items():
	move_desc=[f'**{name}** :']
	if mover.start:
		move_desc.append(f'{inv_x_axis.get(mover.start.x,"??")}{mover.start.y}')
		if mover.target:
			move_desc.append(f'to {inv_x_axis.get(mover.target[0], "??")}{mover.target[1]}')
			if mover.speed and mover.start:
				dx=mover.target[0]-mover.start.x
				dy=mover.target[1]-mover.start.y
				distance = round(sqrt(dx*dx+dy*dy)*ft_per_grid)# TODO: diagonal distance
				move_desc.append(f' - {distance} / {round(mover.speed*ft_per_grid)} ft.')
		else:
			move_desc.append('*No valid target*')
			distance=None
		if not mover.goal:
			move_desc.append('*No available destination*')
		elif mover.target!=mover.goal:
			move_desc.append(f'{inv_x_axis.get(mover.goal[0],"??")}{mover.goal[1]}')
			if mover.block:
				move_desc.append(f'*blocked by* {mover.block}')
			else:
				move_desc.append(f'unreachable')

	elif mover.target:
		move_desc.append(f'Placed at {inv_x_axis.get(mover.target[0], "??")}{mover.target[1]}')
		# TODO: add size here and add size to command. store size as string in mover, (but not in loc)
		#if mover.size:
		#	description.append(f':black_square_button: {mover.size*ft_per_grid}ft.')
		if mover.block:
			err("Unhandled description: blocked at placement")
	else:
		move_desc.append(f'*No place*')
	descriptions.append(space.join(move_desc))

cmd_args=[f'-t {quote if space in name else ""}{name}|{inv_x_axis.get(mover.target[0],"??")}{mover.target[1]}{quote if space in name else ""}' for name, mover in movers.items() if mover.target]
if args.last('verbose',args.last('v',config.get('verbose'))):
	description=nl.join(descriptions)[:6000]
	escaped_quote='\\"'
	cmd_field=f'''-f "Command|`!map {space.join(cmd_args).replace(quote,escaped_quote)[:1000]}`"''' if cmd_args else ''
	dbg_fields=space.join(f'-f "{name}|{nl.join(str(d) for d in mover.dbg)[:1000]}"' for name,mover in movers.items() if mover.dbg)
	not_found_field=f'-f "Unrecognized targets|{", ".join(not_found)}"' if not_found else ''
	return f'embed -title March! -desc "{description}" {cmd_field} {not_found_field} {dbg_fields}'
else:
	return f'echo `!map {space.join(cmd_args)[:1900]}`'
</drac2>