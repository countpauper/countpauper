<drac2>
# test recast

# X axis dict A to CZ should be more than enough, map limit is 99x99
alphabet='ABCDEFGHIJKLMNOPQRSTUVWXYZ'
x_axis={col:idx for idx,col in enumerate(alphabet)}
x_axis.update({f'A{col}':len(alphabet)+idx for idx,col in enumerate(alphabet)})
x_axis.update({f'B{col}':2*len(alphabet)+idx for idx,col in enumerate(alphabet)})
x_axis.update({f'C{col}':3*len(alphabet)+idx for idx,col in enumerate(alphabet)})

argstr='&*&'
args = argparse(argstr)
loc_prefix='Location: '
ovl_number=args.last('#',9,type_=int)
ovl_prefix=f'Overlay{ovl_number or ""}: '
ovl_color=args.last('color','c')
size_prefix='Size: '
height_prefix='Height: '
ft_per_grid=5

caster=None
if c:=combat():
	if (caster_tag:=args.last('c')) and (caster:=c.get_combatant(caster_tag)):
		pass
	elif caster:=c.me:
		caster=c.me
	else:
		caster=c.current

if caster and caster.type!='combatant':
	caster=None

if not caster:
	return f'cast "Spiritual Weapon" {argstr}'
origin_height=None
if c:
	md_key='spiritual_weapon'
	all_metadata=load_json(c.get_metadata(md_key,'{}'))
	caster_metadata=all_metadata.get(caster.name,{})
	if (origin_height:=caster_metadata.get('height')) is not None:
		origin_height=int(origin_height)

# get the current location of the weapon from the overlay or the current location of the caster
origin_pos = None

max_range=None
atk=False
ovl_color='~ff00ff'
caster_notes = []
# range is 60ft from the caster or 20ft from the current position
caster_notes=[n.strip() for n in caster.note.split('|')] if caster.note else []
if overlay_note:=([note[len(ovl_prefix):] for note in caster_notes if note.startswith(ovl_prefix)]+[None])[0]:
	# parse the overlay string
	size_idx=min(idx for idx,c in enumerate(overlay_note) if c.isdigit())	 			# skip shape
	color_idx=size_idx+min(idx for idx,c in enumerate(overlay_note[size_idx:]) if not c.isdigit()) # skip size
	if overlay_note[color_idx]=='~':
		coord_idx=color_idx+7 # ~hex color
	else:
		coord_idx=color_idx+1	# default 1 char id
		for col in ['gy','pk','bn','bk']:	# multi char color ids
			if overlay_note[coord_idx].lower().startswith(col):
				coord_idx=color_idx+len(col)
	ovl_color=overlay_note[color_idx:coord_idx]
	origin_pos=overlay_note[coord_idx:].upper()
	max_range=20//ft_per_grid
	move_explain="As a bonus action on your turn, you can move the weapon up to 20 feet and repeat the attack against a creature within 5 feet of it."
elif location_note:=([note[len(loc_prefix):] for note in caster_notes if note.startswith(loc_prefix)]+[None])[0]:
	origin_pos=location_note
	max_range=60//ft_per_grid
	move_explain="You create a floating, spectral weapon within range [60 feet] that lasts for the duration or until you cast this spell again. "
elif height_note:=([note[len(height_prefix):] for note in caster_notes if note.startswith(height_prefix)]+[None])[0]:
	origin_height=int(height_note)
else:
	move_explain="As a bonus action on your turn, you can move the weapon up to 20 feet and repeat the attack against a creature within 5 feet of it."
ovl_color=args.last('color',ovl_color).lower()

target_pos=None
target=None
target_size=0
target_height=0
if c:
	# get the target location of the weapon from the location of the target or the -m argument
	if target_tag:=args.last('t'):
		target=c.get_combatant(target_tag)
		if not target:
			return f'echo Unknown target `{target_tag}`.'
		# extract the target position from the target's notes
		target_notes=[n.strip() for n in target.note.split('|')] if target.note else []
		if location_note := ([note[len(loc_prefix):] for note in target_notes if note.startswith(loc_prefix)] + [None])[0]:
			target_pos = location_note
		# extract the size from the target's note or by default 1 (M/S/T)
		if size_note :=  ([note[len(size_prefix):] for note in target_notes if note.startswith(size_prefix)] + [None])[0]:
			grid_sizes={'T':1,'S':1,'M':1,'L':2,'H':3,'G':4}
			target_size=grid_sizes.get(size_note[0].upper(),0)
		else:
			target_size=1
		# extract the height from the target's note
		if height_note := ([note[len(height_prefix):] for note in target_notes if note.startswith(height_prefix)] + [None])[0]:
			target_height=int(height_note)

target_pos=args.last('m', target_pos or '').upper()

# explicit move to *Z9 is no check
if '*' in target_pos:
	max_range=None
	target_pos=target_pos.replace('*','')

# convert the text locations to the x,y,z coordinates
origin_height
origin_coord = dict(x=x_axis.get(''.join(c for c in origin_pos if c.isalpha())),
		  y=int(''.join(c for c in origin_pos if c.isdigit())),
		  z=origin_height//ft_per_grid) if origin_pos else None
target_top_left_coord = dict(x=x_axis.get(''.join(c for c in target_pos if c.isalpha())),
		  y=int(''.join(c for c in target_pos if c.isdigit())),
		  z=target_height//ft_per_grid) if target_pos else None

if target_top_left_coord and target_top_left_coord.x is None:
	return f'echo Invalid target position `{target_pos}`'
if origin_coord and origin_coord.x is None:
	origin_coord=None
# approach: find the location around the target's space that is closest to the origin
if target_top_left_coord and origin_coord and target_size:
	# target with a grid size (creature)
	# stand next to it, minimum displacement for 5ft range
	target_bottom_right_coord=dict(x=target_top_left_coord.x+target_size,y=target_top_left_coord.y+target_size,z=target_top_left_coord.z+target_size)
	destination_coord=dict(x=origin_coord.x,y=origin_coord.y,z=origin_coord.z)
	if origin_coord.x<target_top_left_coord.x:
		destination_coord['x']=target_top_left_coord.x-1
	elif origin_coord.x>target_bottom_right_coord.x:
		destination_coord['x']=target_bottom_right_coord.x
	if origin_coord.y<target_top_left_coord.y:
		destination_coord['y']=target_top_left_coord.y-1
	elif origin_coord.y>target_bottom_right_coord.y:
		destination_coord['y']=target_bottom_right_coord.y
	if origin_coord.z<target_top_left_coord.z:
		destination_coord['z']=target_top_left_coord.z-1
	elif origin_coord.z>target_bottom_right_coord.z:
		destination_coord['z']=target_bottom_right_coord.z
	# convert the destination coordinate to a human readable position
else:	# no size (-m), stand on it
	destination_coord=target_top_left_coord
	destination_pos=target_pos

destination_coord['z'] = int(float(args.last('h',args.last('height', destination_coord.z*ft_per_grid))) / ft_per_grid)
destination_pos = f'{list(x_axis.keys())[destination_coord.x]}{destination_coord.y}'
destination_height=destination_coord.z*ft_per_grid # height are in ft, coordinates in grids

# add the change height to position descriptions for embed if relevant
if origin_height!=destination_height:
	origin_pos+=f' {origin_height}'
	destination_pos+=f' {destination_height}'

def compute_distance(a,b):
	if a.z is None:
		delta=[abs(a.x - b.x), abs(a.y - b.y), 0]
	else:
		delta=[abs(a.x - b.x), abs(a.y - b.y), abs(a.z - b.z)]
	if bool(get_svar('trueDistance',get('trueDistance', False))):
		delta.sort()
		diagonal = delta[1]
		straight=delta[2] - diagonal
		return straight + diagonal + diagonal//2	# every 2nd diagonal costs 10ft
	else:
		return max(delta)

# check the range from the origin_coord to the destination_coord
distance = None
if max_range and origin_coord and destination_coord:
	distance=compute_distance(origin_coord, destination_coord) * ft_per_grid
	# sqrt((origin_coord.x-destination_coord.x)**2 + (origin_coord.y-destination_coord.y)**2 + (origin_coord.z-destination_coord.z)**2)*ft_per_grid
	max_range*=ft_per_grid
	if round(distance)>max_range + (ft_per_grid/2):
		return f'echo "Destination {destination_pos} out of range: {distance:.1f} > {max_range}.'
	if distance>0:
		move_field=f'-f "Moving the Spiritual Weapon|from {origin_pos} to {destination_pos} ({distance:.1f} ft.)"'
	else:
		move_field=f'-f "Spiritual Weapon|Stays at {destination_pos}"'
elif destination_pos:
	move_field=f'-f "Placing the Spiritual Weapon|at {destination_pos}"'
else:
	move_field=''

# clean out old overlay:
caster_name = caster.name.replace('"','\"')
attach_effect='Effect: Spiritual Weapon'
caster_notes=[n for n in caster_notes if not n.startswith(ovl_prefix) and not n.startswith(attach_effect)]
if destination_pos:
	caster_notes.append(f'{ovl_prefix}c2{ovl_color}{destination_pos.split(" ")[0]}')
	caster_notes.append(f'{attach_effect} / {caster.name}')
caster.set_note(' | '.join(caster_notes))

# clean spiritual weapon arguments like -m from the argstring
for m in args.get('m'):
	argstr=argstr.replace(f'-m {m}','')

if c:
	if destination_height is not None:
		caster_metadata['height'] = destination_height
	elif 'height' in caster_metadata:
		caster_metadata.pop('height')
	all_metadata[caster.name]=caster_metadata
	c.set_metadata(md_key,dump_json(all_metadata))

attack = ([a for a in caster.attacks if a.name.lower()=='spiritual weapon']+[None])[0]
if not attack:
	cmd=f'i reactcast "{caster_name}" "Spiritual Weapon"  {argstr}'
elif target:
	cmd=f'i aoo "{caster_name}" "{attack.name}" {argstr}'
elif move_field:
	cmd=f'embed -title "{caster.name} moves the Spiritual Weapon." -desc "{move_explain}"'
elif 'end' in argstr:
	# TODO: remove the effect and the overlay when the effect is active but no target or destination
	caster.remove_effect("spiritual weapon")
	return f'embed -title "{caster_name} recalls the Spiritual Weapon."'
else:
	return f'echo No target or destination specified. Use `!help sw` for help.'

return f'{cmd} {move_field}'
</drac2>