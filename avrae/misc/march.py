<drac2>
C=combat()
if not C:
	return f'echo You can only `{ctx.prefix}{ctx.alias}` in combat.'
args = argparse(&ARGS&)

config=load_json(get('march','{}'))
# TODO: read out diagonal configuration. default is Euclidian. setting is "diagonal":ft. use 10 for manhattan or 5, 7.07, 7.5
# when computing distance this is basically just two variations. diagonal multiplied for min(abs(dx),abs(dy))+straight for abs(abs(dx)-abs(dy)) or sqrt(dx*dx+dy*dy)
# TODO: without destination -block is stored in metadata for future use
# -unblock to remove blocks
# -block a1:b2:20 makes it 20 high (always from 0? or a1|0:b2|20?
# store target's speed in metadata or note, use that if not provided as argument

###  some constants
sizes = dict(T=1, S=1, M=1, L=2, H=3, G=4)
sizes[None] = 1
ft_per_grid=5
default_speed=30

alphabet='abcdefghijklmnopqrstuvwxyz'
x_axis={col:idx+1 for idx,col in enumerate(alphabet)}
for mul,prefix in enumerate('abc'):
	x_axis.update({f'{prefix}{col}':(1+mul)*len(alphabet)+idx+1 for idx,col in enumerate(alphabet)})

### attempt to find map bounds
map_rect=dict(x=1,y=1,w=len(x_axis),h=100)
map_combatants=[C.get_combatant('map'),C.get_combatant('dm'),C.get_combatant('lair')]
map_attacks=[a.raw.automation for c in map_combatants if c is not None for a in c.attacks if a.name=='map']
if map_attacks and (map_automation:=map_attacks[0]):
	size_prefix = 'Size: '
	if size_property:=([prop for prop in map_automation[0].get('text').split(' ~ ') if prop.startswith(size_prefix)]+[None])[0]:
		map_rect['w'],map_rect['h']=[int(sp) for sp in size_property[len(size_prefix):].split('x') if sp.isdecimal()]

not_found=[]

### Parse target arguments into a dict of mover_name:dict(c=combatant,s=speed)
size_arg=args.last('size','').upper()[:1] or None
if size_arg not in sizes.keys():
	return f'echo Invalid `-size {size_arg}`'
speed_arg=int(args.last('ft',default_speed,type_=int))
height_arg=args.last('height',None,type_=int)
if targets:=args.get('t'):
	# split the target arguments into target id and config (speed, size and height for now)
	split_targets=[t.split('|') for t in targets]
	target_args = {split_t[0]: dict(speed=int(split_t[1]) if len(split_t)>=2 and split_t[1] and split_t[1].isdecimal() else speed_arg,
									size=split_t[2] if len(split_t)>=3 and split_t[2] and split_t[2] in sizes.keys() else size_arg,
									height=int(split_t[3]) if len(split_t)>=3 and split_t[3] and split_t[3].isdecimal() else height_arg)
				   for split_t in split_targets}
	# find all targeted combatants or groups
	combatants = {t: C.get_combatant(t) or C.get_group(t) for t in target_args.keys() if typeof(t)=='str'}
	movers = dict()

	for t,c in combatants.items():
		if not c:
			not_found.append(t)
		elif c.type == 'group':
			movers.update({member.name:dict(combatant=member,speed=target_args[t].speed, size=target_args[t].size, height=target_args[t].height) for member in c.combatants})
		else:
			movers[c.name]=dict(combatant=c, speed=target_args[t].speed, size=target_args[t].size, height=target_args[t].height)
elif not C.current:
	return f'echo There is no current combatant, use `{ctx.prefix}{ctx.alias} -t <group>`'
elif C.current.type=='group':
	movers={c.name:dict(combatant=c, speed=speed_arg, height=height_arg) for c in C.current.combatants}
else:
	movers={C.current.name:dict(combatant=C.current,speed=speed_arg, height=height_arg)}

if not movers:
	return f'echo The following targets were not found: `{", ".join(not_found)}`'

#return f'echo {target_args} => {movers}'

### create a dict with x,y,s (exclusive) squares of combatants
# and a list of non moving combatants with a location that block targets or goals
locations=dict()
blockers=dict()
loc_prefix = 'Location: '
size_prefix = 'Size: '
height_prefix = 'Height: '
for c in C.combatants:
	notes=[n.strip() for n in c.note.split('|') if n] if c.note else []
	x,y,z,s=None,None,0,None
	for n in notes:
		if n.startswith(loc_prefix):
			location=n[len(loc_prefix):].lower()
			x_loc_str=''.join(c for c in location if c.isalpha())
			x=x_axis.get(x_loc_str)
			y_loc_str=location[len(x_loc_str):]
			y=int(y_loc_str) if y_loc_str.isdecimal() else None
		elif n.startswith(size_prefix):
			s=n[len(size_prefix)].upper()
		elif n.startswith(height_prefix):
			z_loc_str=n[len(height_prefix):]
			z = int(z_loc_str)//ft_per_grid if z_loc_str.isdecimal() else z
	if x is not None and y is not None:
		locations[c.name] = dict(x=x, y=y, z=z, s=s, w=sizes[s], h=sizes[s])

# convert the movers list to a dictionary per mover with all necessary data
old_movers={k:v for k,v in movers.items()}
movers={n:dict(combatant=m.combatant,
			   start=locations.get(n),
			   speed=(m.get('speed') or default_speed)/ft_per_grid,
			   size=(s:=m.get('size') or locations.get(n,dict()).get('s')),
			   z=(mover_height if (mover_height:=m.get('height')) is not None else locations.get(n,dict()).get('z',0)*ft_per_grid)//ft_per_grid,
			   w=sizes[s],
			   h=sizes[s],
			   goal=None,
			   destination=None,
			   block=None,
			   dbg=[]) for n,m in movers.items()}

# add all non moving map characters to the list of blockers
for name,loc in locations.items():
	if not name in movers.keys():
		blockers[name] = dict(x=loc.x, y=loc.y, z=loc.z, w=loc.w, h=loc.h)

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
				area_rect=dict(x=loc.x, y=loc.y, w=loc.w, h=loc.h)
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

min_w=min(m.w for m in movers.values())
min_h=min(m.h for m in movers.values())
for area in zones.approach:
	# plot goal locations
	for x in range(area.x-min_w, area.x+area.w):
		goals.append((x, area.y-min_h))
		goals.append((x+min_w, area.y+area.h))
	for y in range(area.y-min_h, area.y+area.h):
		goals.append((area.x-min_w, y+min_h))
		goals.append((area.x+area.w, y))

# Add height to the goals. if not specified the current height is maintained
# the height range of all movers is used to determine available goals
if height_arg:
	z_arg=height_arg//ft_per_grid
	goal_z=(z_arg, z_arg+max(mover.h for mover in movers.values()))
else:
	goal_z=(min(mover.z for mover in movers.values()),max(mover.z+mover.h for mover in movers.values()))

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
	# uses minimum movers size as a pre-check. bigger sized may fail at path planning post-check
	if any(goal[0]+min_w>b.x and goal[1]+min_h>b.y and goal_z[1]>b.z and
		   goal[0]<b.x+b.w and goal[1]<b.y+b.h and goal_z[0]<b.z+b.h for b in blockers.values()):
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


#return f'echo {unclipped_goals} @ {goal_z} in {map_rect} and not in {blockers} => {goals}'

### Assign movers to goals, in order of movers, sorted preferred target by distance
for mover in movers.values():
	if not mover.goal:
		if not mover.start and goals:	# place
			mover['goal']=goals.pop(0)
		else:
			best_idx, best_dist=None, 0
			for idx,t in enumerate(goals):
				dx=mover.start.x - t[0]
				dy=mover.start.y - t[1]
				dz=mover.start.z - mover.z
				sqr_d=dx*dx + dy*dy	+dz*dz # TODO different speed diagonals
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

def length(x, y, z=0):
	# todo: diagonal distance option
	return sqrt(x*x + y*y + z*z)

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
#return f'echo {movers}'

# move towards goals with speed and compute the destination until blocked or out of movement
for name, mover in movers.items():
	if mover.destination:
		continue
	# render pre path, track movement remaining, reverse order
	if mover.start:
		path = [(mover.start.x, mover.start.y, mover.start.z)]
		movement=mover.speed
		if pre_path:
			for dx,dy in pre_path:
				if movement<0.5:
					break
				path.insert(0,(path[0][0] + dx, path[0][1] + dy, path[0][2]))
				movement-=length(dx, dy)
				mover['goal']=path[0]

		# add approach path
		if mover.goal:
			pos=path[0]
			# compute normalized direction vector
			dx, dy, dz=mover.goal[0] - pos[0], mover.goal[1]-pos[1], goal_z[0]-pos[2]
			vector_length = length(dx, dy, dz)
			if vector_length>0.1:
				dx/=vector_length
				dy/=vector_length
				dz/=vector_length
				path_length=min(movement,vector_length)
				# render all grids from end to pos the path, path may contain duplicates due to rounding but that's only a small performance problem
				path=[(round(pos[0]+dx*step),round(pos[1]+dy*step), round(pos[2]+dz*step)) for step in range(round(path_length),0,-1)]+path
	elif mover.goal:	# place
		path=[(mover.goal[0], mover.goal[1], mover.z)]
	else:
		path=[]
	# TODO render post path, is there really a use case?

	# mover['dbg'].append(path)
	# find furthest non-blocked point along the path
	# mover.dbg.append(f'path {path}')
	for p in path:
		# mover.dbg.append(f'blocked at {p} by { {f"{n}({b.x},{b.y},{b.w},{b.h})":p[0] + mover.w > b.x and p[1] + mover.h > b.y and p[0] < b.x + b.w and p[1] < b.y + b.h for n,b in blockers.items()} }')
		if p[0] < map_rect.x or p[1] < map_rect.y or p[0]+mover.w > map_rect.x + map_rect.w or p[1]+mover.h > map_rect.y + map_rect.h:
			mover['block'] = 'map'
			continue
		for blocker, block in blockers.items():
			if p[0] + mover.w > block.x and p[1] + mover.h > block.y and p[2] + mover.h > block.z \
					and p[0] < block.x + block.w and p[1] < block.y + block.h and p[2] < block.z + block.h:
				mover['block'] = blocker
				break
		else:	# not blocked, move to p
			mover['destination'] = p
			blockers[name]=dict(x=mover.destination[0], y=mover.destination[1], z=mover.destination[2], w=mover.w, h=mover.h)
			break
	else:	# end of path, stay in position
		mover['destination'] = None
		if mover.start:
			blockers[name] = dict(x=mover.start.x, y=mover.start.y, z=mover.start.z, w=mover.w, h=mover.h)
	# return f'echo path `{path}`  to @`{goal_z}`for `{mover}`'

### generate verbose output
inv_x_axis={val:s.upper() for s,val in x_axis.items()}
space,nl,quote=' ','\n','"'

def CoordString(x, y=None, z=None):
	if typeof(x) == 'SafeDict':
		return CoordString(x.get('x', 0), x.get('y', 0), x.get('z', None))
	elif typeof(x)=='SafeList' or typeof(x)=='tuple':
		if len(x)>2:
			return CoordString(x[0], x[1], x[2])
		else:
			return CoordString(x[0], x[1])
	else:
		return f'{inv_x_axis.get(x,"??")}{y}' + (f' :arrow_up: {mover.start.z*ft_per_grid}ft' if z is not None else '')

### create arguments for all movers
for name, mover in movers.items():
	props=dict()
	if mover.destination:
		props[1]=CoordString(mover.destination[:2])
	if mover.size and (not mover.start or mover.size!=mover.start.s):
		props[2]=mover.size
	if (not mover.start and mover.destination[2]!=0) or (mover.destination[2] !=mover.start.z):
		props[5]=int(mover.destination[2]*ft_per_grid)
	if props:
		mover['args']=[name]+[str(props.get(idx,'')) for idx in range(1,1+max(props.keys()))]
	else:
		mover['args']=None

### generate embed text
output_args=['|'.join(mover.args) for mover in movers.values() if mover.args]
cmd_args=[f'-t {quote if space in arg else ""}{arg}{quote if space in arg else ""}' for arg in output_args]
if args.last('verbose',args.last('v',config.get('verbose'))):
	# generate verbose descriptions
	descriptions = []
	for name, mover in movers.items():
		move_desc = [f'**{name}** :']
		if mover.start:
			move_desc.append(CoordString(mover.start))
			if mover.destination:
				move_desc.append(f'to {CoordString(mover.target)}')
				if mover.speed and mover.start:
					dx = mover.destination[0] - mover.start.x
					dy = mover.destination[1] - mover.start.y
					distance = round(sqrt(dx * dx + dy * dy) * ft_per_grid)  # TODO: diagonal distance
					move_desc.append(f' - {distance} / {round(mover.speed * ft_per_grid)} ft.')
			else:
				move_desc.append('*No valid target*')
				distance = None
			if not mover.goal:
				move_desc.append('*No available destination*')
			elif mover.destination[:2] != mover.goal[:2]:
				move_desc.append(CoordString(mover.goal))
				if mover.block:
					move_desc.append(f'*blocked by* {mover.block}')
				else:
					move_desc.append(f'unreachable')

		elif mover.destination:
			move_desc.append(f'Placed at {CoordString(mover.destination)}')
			# TODO: add size here and add size to command. store size as string in mover, (but not in loc)
			if mover.block:
				err("Unhandled description: blocked at placement")
		else:
			move_desc.append(f'*No place*')
		descriptions.append(space.join(move_desc))

	description=nl.join(descriptions)[:6000]
	escaped_quote='\\"'
	cmd_field=f'''-f "Command|`!map {space.join(cmd_args).replace(quote,escaped_quote)[:1000]}`"''' if cmd_args else ''
	dbg_fields=space.join(f'-f "{name}|{nl.join(str(d) for d in mover.dbg)[:1000]}"' for name,mover in movers.items() if mover.dbg)
	not_found_field=f'-f "Unrecognized targets|{", ".join(not_found)}"' if not_found else ''
	return f'embed -title March! -desc "{description}" {cmd_field} {not_found_field} {dbg_fields}'
else:
	return f'echo `!map {space.join(cmd_args)[:1900]}`'
</drac2>