<drac2>
# timeout to remove using techo, which argument? any number? first number? as a field obviously
# var display type, just gets a cvar and shows it (for speed?)
# auto cvar?
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
	if ap.last('i'):
		stat.append(c.current)
	for t in ap.get('t'):
		if t in args:
			args.remove(t)
		if t=='*':
			stat+=[combatant for combatant in c.combatants if combatant.name.lower() and combatant.hp is not None]
		elif g:=c.get_group(t):
			stat+=g.combatants
		elif target:=c.get_combatant(t):
			stat.append(target)
		else:
			stat.append(None)
	if not stat and c.me:
		stat=[c.me]
else:
	if ap.get('i'):
		return f'echo `-i` argument is only supported in initiative.'
	if ap.get('t'):
		return f'echo `-t` argument is only supported in initiative.'
if not stat:
	stat=[ch]
elif not c.me in stat:
	ch=None

config=load_json(get_gvar('f9fd35a8-1c8e-477c-b66e-2eeee09a4735'))
config_fields=config.fields
# TODO get this field list from 1) command line, 2) uvar 3) cvar (if ch)

# build the configuration (field_list) of fields to display
# create the field list from arguments
field_list=[]
command_list=['set','default','var','-i','-t']	# auto added as field
for a in args:
	if a[0] in '-+' or a in command_list or a.isdecimal():
		continue
	field_list.append(a)

default_fields=[f for f,prop in config_fields.items() if prop.get('default',False)]
if ap.last('default'):
	field_list=default_fields+field_list

# load fields from cvar if no fields are specified without a + or -
cvar_name=f'{ctx.alias}_items'
uvar_name = f'default_{ctx.alias}_items'
if not field_list:
	if c := character():
		c.delete_cvar(uvar_name)
	field_list=get(uvar_name,dump_json(default_fields))
	if ch:
		field_list=get(cvar_name,field_list)
	field_list=load_json(field_list)

bag_items = None
coins = []
if ch:
	if bags:=get('bags'):
		bags = load_json(bags)
		# get coin items used from any 'coin' bag
		coins=list(([b for n,b in bags if n.lower().startswith('coin')]+[{}])[0].keys())
		coins.reverse()
		# flatten bags into one item dict, lower names
		bag_items={}
		for _,b in bags:
			for i,q in b.items():
				i=i.lower()
				bag_items[i]=bag_items.get(i,0)+q

# +field arguments are additional, no matter the source
field_list+=[a[1:] for a in args if a[0]=='+']
# and so are digits for the timeout
field_list+=[a for a in args if a.isdecimal()]

# Add configuration fields for which the character has a matching cc or item
auto='auto'
if auto in field_list and ch:
	field_list.pop(index:=field_list.index(auto))
	# pre grab so each cc is added only once
	consumable_names={cc.name for cc in ch.consumables}
	item_names=list(bag_items.keys())
	for label, field in config_fields.items():
		if field.get('auto',True) and label not in field_list:
			if (field_cc:=field.get('cc')) and field_cc in consumable_names:
				consumable_names.remove(field_cc)
				field_list.insert(index,label)
				index+=1
			if (field_item:=field.get('item')) and (match_item:=([i for i in item_names if field_item in i]+[None])[0]):
				item_names.remove(match_item)
				field_list.insert(index,label)
				index+=1

# replace all "cc..." fields with custom cc configurations
custom_displays=['cc','item']
for idx in range(len(field_list)):
	f=field_list[idx]
	if not typeof(f)=='str':
		continue
	for custom in custom_displays:
		if f.startswith(custom):
			f=f[len(custom):].strip(' ,()')
			# arguments are <CC>,[<icon>],[<display type>]
			sub_name,icon,display=(f.split(',')+[None,None,None])[:3]
			if not sub_name:
				continue
			# fuzzy matching with known CCs
			if ch and custom=='cc':
				for cc in ch.consumables:
					if cc.name.lower().startswith(sub_name.lower()):
						sub_name=cc.name
						break
				display = display or custom
				field_list[idx] = dict(display=display, cc=sub_name, icon=icon)
			elif ch and custom=='item':
				# TODO: last argument unused
				item_name=([i for i in bag_items.keys() if sub_name.lower() in i]+[sub_name])[0]
				field_list[idx] = dict(display=custom, item=item_name, icon=icon)
			break

# -field arguments are removed
for a in args:
	if a.startswith('-cc'):
		cc=a[3:].strip(' ,()').lower()
		field_list=[f for f in field_list if typeof(f)=='str' or not f.get('cc','').lower().startswith(cc)]
	elif a.startswith('-item'):
		item = a[5:].strip(' ,()').lower()
		field_list = [f for f in field_list if typeof(f) == 'str' or not f.get('item', '').lower().startswith(item)]
	elif a[:1]=='-' and (f:=a[1:]) in field_list:
		field_list.remove(f)

# store or dump output
if ap.last('set') and ch:
	ch.set_cvar(cvar_name,dump_json(field_list))

if ap.last('var'):
	if ch:
		return f'echo `!cvar {cvar_name} {dump_json(field_list)}`'
	else:
		return f'echo `!uvar {uvar_name} {dump_json(field_list)}`'

field_list=[config_fields.get(field.lower(),field) if typeof(field)=='str' else field for field in field_list]
out=[]
if len(stat)>1:
	out.append(f'{len(stat)} combatants:')
	if 'name' not in field_list:
		field_list=[config_fields.name]+field_list

timeout=None
for s in stat:
	if not s:
		continue
	field=[]
	hidden=not s.hp_str().upper().endswith('HP>')
	for f in field_list:
		if not f:
			continue
		if typeof(f)=='str':
			if f.isdecimal():
				timeout=int(f)
				hidden = False
			else:
				field.append(f)
			continue
		display=f.get('display')
		icon=f.get('icon','')
		if icon and typeof(icon)=='str':
			icon+=' '
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
		elif display=='bar' and s.hp:
			bar_length=f.get('length',10)
			div = 1 + (s.max_hp // bar_length)
			field.append(f'{icon * (s.hp // div)}{f.get("alticon",":black_large_square:") * ((s.max_hp - s.hp) // div)} {"" if div == 1 else f"[x{div}]"}')
		elif display=='ac' and s.ac and not hidden:
			field.append(f'{icon}{s.ac}')
		elif display=='spell' and s.spellbook and not hidden:
			label = [None]+f.get('label')
			bubble=f.get('type').lower()=='bubble'
			sb=s.spellbook
			ss=[f'{label[lvl]}{sb.slots_str(lvl)[3:] if bubble else f"{sb.get_slots(lvl)}/{sb.get_max_slots(lvl)}"}' for lvl in range(1,10) if sb.get_max_slots(lvl)]
			field.append(icon+' '.join(ss))
		elif display=='label':
			field.append(icon+f.get('text',''))
		elif display=='class':
			if cr:=s.levels.get('Monster'):	# mosnter class, show as CR
				if cr<1:
					cr=f'1/{int(1/cr)}'
				field.append(f'{icon}CR {cr}')
			else:
				sub_class = load_json(get('subclass', '{}')) if ch else {}
				sub_class = {cls[:-5]: sub for cls, sub in sub_class.items()}
				levels=[f'{sub_class[cls]} {cls} `{level}`' if cls in sub_class else f'{cls} `{level}`' for cls,level in s.levels]
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
				field.append(f'{icon}`{xp}`/{max_xp}')
		elif display is None:
			field.append(icon)
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
		elif display=='effect' or display=='effects':
			effects=[]
			for e in s.effects:
				if conds := [c_str for cond, c_str in config.conditions.items() if cond in e.name.lower()]:
					effects.append(''.join(conds))
				elif e.conc and e.children:
					effects.append(':brain:')
				else:
					effects.append(e.name)
			if effects:
				field.append(f'{icon}{", ".join(effects)}')
		elif display=='coins' and bag_items and coins:
			field.append(f'{icon} {" ".join(f"`{q}`{c}" for c in coins if (q:=bag_items.get(c,0)))}')
		elif display == 'gold' and bag_items:
			conversion={'pp':10.0,'ep':5.0,'gp':1.0,'sp':0.1,'cp':0.01}
			gold=sum(conversion.get(k,0)*q for k,q in bag_items.items())
			field.append(f'{icon} {gold:.2f}'.rstrip('0').rstrip('.'))
		elif display=='flag' and ch and ch.cc_exists(cc:=f['cc']):
			field.append(icon * ch.get_cc(cc))
		elif display=='cc' and ch and ch.cc_exists(cc:=f['cc']):
			if 	(max_cc:=f.get('max',ch.get_cc_max(cc)))<0:
				max_cc=ch.get_cc_max(cc)-max_cc
			min_cc=f.get('min',ch.get_cc_min(cc))
			cc_val=ch.get_cc(cc)
			if cc_val>=min_cc and cc_val<=max_cc:
				field.append(f'{icon}{ch.cc_str(cc)}')
		elif display=='item' and bag_items:
			item=f.get('item','').lower()
			amount=sum(q for i,q in bag_items.items() if any(w.lower().startswith(item) for w in i.split()))
			field.append(f'{icon}{amount}')
	if field:
		out.append(' '.join(field))
	else:
		out.append('*Invalid field selection.*')


if out:
	nl = '\n'
	return f'{"echo" if timeout is None else f"techo {timeout}"} {nl.join(out)}'
else:
	return f'echo *Invalid target selection.*'
</drac2>