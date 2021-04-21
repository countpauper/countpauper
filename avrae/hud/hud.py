<drac2>
# if config string is a gvar, then that gvar overrides the default gvar
# config (built-in) with arguments. auto argument adds all ccs that match ccs, -removed +adds any bla resets, 'default' adds name hp ac spell
# items (gp,sp, arrow, bolt, healing potion) from bag
# certain basic effects? (unconscious, restrained, grappled)
# certain class effects: bladesong/rage/wildshape?
# automatic support for current player's wildshape/polymorph state?

args=[a.lower() for a in &ARGS&]
ap=argparse(args)

# Get statblocks, ch is character for CCs and other things not available from combatants
# stat is a list of combatants for hp, ac other simple stuff
ch=character()
stat=[]
if c:=combat():
	for t in ap.get('i') + ap.get('t'):
		if t=='True':
			stat.append(c.current)
		elif g:=c.get_group(t):
			stat+=g.combatants
		elif target:=c.get_combatant(t):
			stat.append(target)
else:
	if ap.get('i'):
		return f'echo `-i` argument is only supported in initiative.'
	if ap.get('t'):
		return f'echo `-t` argument is only supported in initiative.'
if not stat:
	stat=[ch]
elif not ch in stat:
	ch=None

config=load_json(get_gvar('f9fd35a8-1c8e-477c-b66e-2eeee09a4735'))
# TODO get this field list from 1) command line, 2) uvar 3) cvar (if ch)

# build the configuration (field_list) of fields to display
# create the field list from arguments
field_list=[]
for a in args:
	if a[0]=='-':
		continue
	if a[0]=='+':
		continue
	if a in config.keys():
		field_list.append(a)

default_fields=["name", "hp", "ac", "spell"]
if ap.last('default'):
	field_list=default_fields+field_list

cvar_name='hud_fields'
if not field_list:
	uvar_name='default_hud_fields'
	if c := character():
		c.delete_cvar(uvar_name)
	field_list=get(uvar_name,dump_json(default_fields))
	if ch:
		field_list=get(cvar_name,field_list)
	field_list=load_json(field_list)

if ap.last('auto'):
	pass 	# TODO: scan all CCs for config match

# +field arguments are additional, no matter the source
field_list+=[a[1:].lower() for a in args if a[0]=='+']
# -field arguments are removed
for label in config.keys():
	if f'-{label}' in args:
		field_list.remove(label)

if ap.last('set'):
	character().set_cvar(cvar_name,dump_json(field_list))

# retrieve full config for field reference strings
field_list=[config.get(field) if typeof(field)=='str' else field for field in field_list]

out=[]
for s in stat:
	if not s:
		continue
	field=[]
	for f in field_list:
		if not f:
			continue

		display=f.get('display')
		icon=f.get('icon','')
		icon+=' ' if icon else ''
		if display=='name':
			if f.get('bold'):
				field.append(f'{icon}**{s.name}**')
			else:
				field.append(f'{icon}{s.name}')
		elif display=='hp' and s.hp:
			# TODO: field has death field then show that icon, field has dying icon then show those
			if s.hp>0:
				field.append(f'{icon}{s.hp_str()}')
			elif ch:
				if ch.death_saves.is_dead():
					field.append(f':skull_crossbones:  {ch.death_saves}')
				else:
					field.append(f':heart: {ch.death_saves}')
			else:
				field.append(':skull_crossbones:')
		elif display=='ac' and s.ac:
			field.append(f'{icon}{s.ac}')
		elif display=='spell' and s.spellbook:
			label = [None]+f.get('label')
			bubble=f.get('type').lower()=='bubble'
			sb=s.spellbook
			ss=[f'{label[lvl]}{sb.slots_str(lvl)[3:] if bubble else f"{sb.get_slots(lvl)}/{sb.get_max_slots(lvl)}"}' for lvl in range(1,10) if sb.get_max_slots(lvl)]
			field.append(icon+' '.join(ss))
		elif display=='class':
			if cr:=s.levels.get('Monster'):	# mosnter class, show as CR
				if cr<1:
					cr=f'1/{int(1/cr)}'
				field.append(f'{icon}CR {cr}')
			else:
				subclass = load_json(get('subclass', '{}')) if ch else {}
				subclass = {cls[:-5]: sub for cls, sub in subclass.items()}
				levels=[f'{subclass[cls]} {cls} `{level}`' if cls in subclass else f'{cls} `{level}`' for cls,level in s.levels]
				field.append(icon+'/'.join(lvl for lvl in levels))
		elif display=='xp' and ch:
			cc='Experience'
			xp=ch.get_cc(cc) if ch.cc_exists(cc) else 0
			xp_table=[0,300,900,2700,6500,14000,23000,34000,48000,64000,85000,100000,120000,140000,165000,195000,225000,265000,305000,355000,None]
			max_xp=xp_table[ch.levels.total_level]
			if max_xp is None:
				field.append(f'{icon}`{xp}`')
			elif f.get('%'):
				prev_max=xp_table[ch.levels.total_level-1]
				fraction=(xp-prev_max)/(max_xp-prev_max)
				field.append(f'{icon}`{int(fraction*100)}`%')
			else:
				field.append(f'{icon}`{xp}`/`{max_xp}`')
		elif display=='hd' and ch:
			hds=[6,8,10,12]
			total_hd,max_hd=0,0
			for hd in hds:
				cc=f'Hit Dice (d{hd})'
				if ch.cc_exists(cc):
					total_hd+=ch.get_cc(cc)
					max_hd+=ch.get_cc_max(cc)
			if max_hd:
				field.append(f'{icon}`{total_hd}`/`{max_hd}`')
		elif display=='flag' and ch and ch.cc_exists(cc:=f['cc']):
			field.append(icon * ch.get_cc(cc))
		elif display=='cc' and ch and ch.cc_exists(cc:=f['cc']):
			if 	(max_cc:=f.get('max',ch.get_cc_max(cc)))<0:
				max_cc=ch.get_cc_max(cc)-max_cc
			min_cc=f.get('min',ch.get_cc_min(cc))
			cc_val=ch.get_cc(cc)
			if cc_val>=min_cc and cc_val<=max_cc:
				field.append(f'{icon}{ch.cc_str(cc)}')

	out.append(' '.join(field))
nl='\n'
return f'echo {nl.join(out)}'
</drac2>