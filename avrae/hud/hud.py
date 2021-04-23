<drac2>
# list sub command that shows all the options, with an icon for the type (item or counter ammo especially)
# counters of my own characters (channel harness, sorcery, arcanee, bladesong, ki)
# auto inserts at auto position
# main class counters
# if config string is a gvar, then that gvar overrides the default gvar
# certain basic effects? (unconscious, restrained, grappled)
# certain class effects: bladesong/rage/wildshape?
# automatic support for current player's wildshape/polymorph state?
# race counters

args=[a for a in &ARGS&]
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
			stat.append(None)
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
command_list=['set','auto','default','var','-i','-t']
for a in args:
	if a[0] in '-+' or a in command_list:
		continue
	field_list.append(a)

default_fields=["name", "hp", "ac", "spell"]
if ap.last('default'):
	field_list=default_fields+field_list

# load fields from cvar if no fields are specified without a + or -
cvar_name='hud_fields'
if not field_list:
	uvar_name='default_hud_fields'
	if c := character():
		c.delete_cvar(uvar_name)
	field_list=get(uvar_name,dump_json(default_fields))
	if ch:
		field_list=get(cvar_name,field_list)
	field_list=load_json(field_list)

# Add configuration fields for which the character has a matching cc
if ap.last('auto') and ch:
	# pre grab so each cc is added only once
	consumable_names={cc.name for cc in ch.consumables}
	for label, field in config.items():
		if label not in field_list and (field_cc:=field.get('cc')):
			if field_cc in consumable_names:
				consumable_names.remove(field_cc)
				field_list.append(label)

# +field arguments are additional, no matter the source
field_list+=[a[1:] for a in args if a[0]=='+']

# replace all "cc..." fields with custom cc configurations
for idx in range(len(field_list)):
	f=field_list[idx]
	if typeof(f)=='str' and f.startswith('cc'):
		f=f[2:].strip(' ,()')
		# arguments are <CC>,[<icon>],[<display type>]
		cc_name,icon,display=(f.split(',')+[None,None,None])[:3]
		if not cc_name:
			continue
		# fuzzy matching with known CCs
		if ch:
			for cc in ch.consumables:
				if cc.name.lower().startswith(cc_name.lower()):
					cc_name=cc.name
					break

		display=display or 'cc'
		field_list[idx]=dict(display=display,cc=cc_name,icon=icon)

# -field arguments are removed
for a in args:
	if a[:3]=='-cc':
		cc=a[3:].strip(' ,()').lower()
		field_list=[f for f in field_list if typeof(f)=='str' or not f.get('cc','').lower().startswith(cc)]
	elif a[:1]=='-' and (f:=a[1:]) in field_list:
		field_list.remove(f)

# store or dump output
if ap.last('set') and ch:
	ch.set_cvar(cvar_name,dump_json(field_list))

if ap.last('var'):
	if ch:
		return f'echo `!cvar {dump_json(field_list)}`'
	else:
		return f'echo `!uvar {dump_json(field_list)}`'

if ch:
	bags=get('bags')
	bags=load_json(bags) if bags else None
else:
	bags=None

# retrieve full config for field reference strings
field_list=[config.get(field.lower(),field) if typeof(field)=='str' else field for field in field_list]

out=[]
for s in stat:
	if not s:
		continue
	field=[]
	for f in field_list:
		if not f:
			continue
		if typeof(f)=='str':
			field.append(f)
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
		elif display=='item' and bags:
			item=f.get('item','').lower()
			amount=sum(q for _,b in bags for i,q in b.items() if any(w.lower().startswith(item) for w in i.split()))
			field.append(f'{icon}{amount}')
	if field:
		out.append(' '.join(field))
	else:
		out.append('*Invalid field selection.*')
if out:
	nl = '\n'
	return f'echo {nl.join(out)}'
else:
	return f'echo *Invalid target selection.*'
</drac2>