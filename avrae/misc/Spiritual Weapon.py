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
	return f'echo No valid caster found. Either join initiative, use it for the current character (groups not supported) or use `-c "<caster>"`.'

# get the current location of the weapon from the overlay or the current location of the caster
origin_pos = None
max_range=None
atk=False
ovl_color='~ff00ff'
caster_notes = []
if caster:
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
		max_range=20//5
		move_explain="As a bonus action on your turn, you can move the weapon up to 20 feet and repeat the attack against a creature within 5 feet of it."
	elif location_note:=([note[len(loc_prefix):] for note in caster_notes if note.startswith(loc_prefix)]+[None])[0]:
		origin_pos=location_note
		max_range=60//5
		move_explain="You create a floating, spectral weapon within range [60 feet] that lasts for the duration or until you cast this spell again. "
	else:
		move_explain="As a bonus action on your turn, you can move the weapon up to 20 feet and repeat the attack against a creature within 5 feet of it."
ovl_color=args.last('color',ovl_color).lower()

target_pos=None
target=None
target_size=0
if c:
	# get the target location of the weapon from the location of the target or the -m argument
	if target_pos:=args.last('m','').upper():
		pass

	if target_tag:=args.last('t'):
		target=c.get_combatant(target_tag)
		if not target:
			return f'echo Unknown target `{target_tag}`.'
		if not target_pos:
			target_notes=[n.strip() for n in target.note.split('|')] if target.note else []
			if location_note := ([note[len(loc_prefix):] for note in target_notes if note.startswith(loc_prefix)] + [None])[0]:
				target_pos = location_note
			if size_note :=  ([note[len(size_prefix):] for note in target_notes if note.startswith(size_prefix)] + [None])[0]:
				grid_sizes={'T':1,'S':1,'M':1,'L':2,'H':3,'G':4}
				target_size=grid_sizes.get(size_note[0].upper(),0)
			else:
				target_size=1

# explicit move to *Z9 is no check
if '*' in target_pos:
	max_range=None
	target_pos=target_pos.replace('*','')

# convert the text locations to the x,y coordinates
op = dict(x=x_axis.get(''.join(c for c in origin_pos if c.isalpha())),
		  y=int(''.join(c for c in origin_pos if c.isdigit()))) if origin_pos else None
tl = dict(x=x_axis.get(''.join(c for c in target_pos if c.isalpha())),
		  y=int(''.join(c for c in target_pos if c.isdigit()))) if target_pos else None

if tl and op and target_size:
	# target with a grid size (creature)
	# stand next to it, minimum displacement for 5ft range
	br=dict(x=tl.x+target_size,y=tl.y+target_size)
	tp=dict(x=op.x,y=op.y)
	if op.x<tl.x:
		tp['x']=tl.x-1
	elif op.x>br.x:
		tp['x']=br.x
	if op.y<tl.y:
		tp['y']=tl.y-1
	elif op.y>br.y:
		tp['y']=br.y

	destination_pos=f'{list(x_axis.keys())[tp.x]}{tp.y}'
else:	# no size (-m), stand on it
	tp=tl
	destination_pos=target_pos

# check range
if max_range and op and tp:
	distance=sqrt((op.x-tp.x)**2 + (op.y-tp.y)**2)*5.0
	max_range*=5
	if round(distance)>max_range:
		return f'echo "Destination {destination_pos} out of range: {distance:.1f} > {max_range}.'
	if distance>0:
		move_field=f'-f "Moving the Spiritual Weapon|from {origin_pos} to {destination_pos} [{distance:.1f} ft.]"'
	else:
		move_field=f'-f "Spiritual Weapon|Stays at {destination_pos}"'
elif destination_pos:
	move_field=f'-f "Placing the Spiritual Weapon|at {destination_pos}"'
else:
	move_field=''

# clean out old overlay:
if caster:
	caster_notes=[n for n in caster_notes if not n.startswith(ovl_prefix)]
	if destination_pos:
		caster_notes.append(f'{ovl_prefix}c2{ovl_color}{destination_pos}')
	caster.set_note(' | '.join(caster_notes))

# clean spiritual weapon arguments like -m from the argstring
for m in args.get('m'):
	argstr=argstr.replace(f'-m {m}','')

if caster:
	effect_name = 'spiritual weapon'
	# instead of effect, could ook for actual attack
	# TODO: instead of aoo and reactcast, could use normal a and cast if current.name==caster.name
	if caster.get_effect(effect_name) is None:
		cmd=f'i reactcast "{caster.name}" "Spiritual Weapon"  {argstr}'
	elif target:
		cmd=f'i aoo "{caster.name}" "Spiritual Weapon" {argstr}'
	elif move_field:
		cmd=f'embed -title "{caster.name} moves the Spiritual Weapon." -desc "{move_explain}"'
	else:
		# TODO: remove the effect and the overlay when the effect is active but no target or destination
		caster.remove_effect(effect_name)
		return f'embed -title "{caster.name} recalls the Spiritual Weapon."'
else:
	cmd=f'cast "Spiritual Weapon"'

return f'{cmd} {move_field}'
</drac2>