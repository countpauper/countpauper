<drac2>
C=combat()
if not C:
	return f'echo Colors can only be set in combat.'


colors=dict(
	clear=dict(tag=None, icon=':no_entry_sign:'),
	black=dict(tag='bk',icon=':black_circle:'),
	white=dict(tag='w', icon=':white_circle:'),
	red=dict(tag='r', icon=':red_circle:'),
	yellow=dict(tag='y', icon=':yellow_circle:'),
	green=dict(tag='g', icon=':green_circle:'),
	blue=dict(tag='b', icon=':blue_circle:'),
	purple=dict(tag='p', icon=':purple_circle:'),
	brown=dict(tag='bn', icon=':brown_circle:'),
	orange=dict(tag='o', icon=':orange_circle:')
)

ring_prefix=dict(T='c2',S='c3', M='c3', L='co6', H='c8', G='co11')

var_name='combat_colors'
conditions=load_yaml(get_svar(var_name,get(var_name,dict(
	dying= 'black', dead= 'black', petrified= 'black',
	prone= 'purple', unconscious= 'purple', blinded= 'purple', restrained= 'purple', paralyzed= 'purple', stunned= 'purple', dodge='purple',
	sanctuary='red',
	charmed= 'yellow', frightened= 'yellow',
	grappled= 'blue', incapacitated= 'blue',
	poisoned= 'green',
	invisible= 'white',
	concentrating= 'orange', ready= 'orange'
))))
if typeof(conditions)!='SafeDict':
	return f'echo Invalid condition configuration ```{dump_yaml(conditions)}```'

def ParseMapNotes(notes):
	if not notes:
		return dict()
	parsed=dict()
	for note in notes.split('|'):
		key,val = note.strip().split(':')
		parsed[key] = val.strip()
	return parsed

def SerializeMapNotes(parsed):
	return " | ".join(f'{k}: {v}' for k,v in parsed.items())

def IsMonster(creature):
	return bool(creature.levels.get('Monster'))

def IsPositive(value):
	if typeof(value)=='str':
		value=roll(value)
	if typeof(value)=='SafeList':
		value=len(value)
	if typeof(value)=='bool':
		return value
	if typeof(value)=='int':
		return value>0
	err(f'Unsupported effect bonus type {typeof(value)}')

def IsNegative(value):
	if typeof(value)=='str':
		value=roll(value)
	if typeof(value)=='int':
		return value<0
	err(f'Unsupported effect penalty type {typeof(value)}')

def IsBuf(e):
	neutral_props=['attack_advantage','to_hit_bonus','damage_bonus','ac_bonus','max_hp_value','max_hp_bonus','save_bonus', 'check_bonus']
	good_props=['magical_damage','silvered_damage','resistances','immunities','save_adv']
	if any(IsPositive(v) for b,v in e.effect.items() if b in good_props+neutral_props):
		return True
	return False

def IsDebuf(effect):
	bad_props='vulnerabilities','ignored_resistances','save_dis'
	if any(IsPositive(v) for p,v in e.effect.items() if p in bad_props):
		return True
	neutral_props=['attack_advantage','to_hit_bonus','damage_bonus','ac_bonus','max_hp_value','max_hp_bonus','save_bonus',	'check_bonus']
	if any(IsNegative(v) for p,v in e.effect.items() if p in neutral_props):
		return True
	return False

def CombatantEffects(combatant):
	effects = [condition for condition in conditions.keys() if combatant.get_effect(condition) and condition!='dying']
	if combatant.hp is not None:
		if (combatant.hp == 0) and (not IsMonster(combatant)):
			effects.append('dying')
		elif combatant.hp <= 0:
			effects.append('dead')
	if any(e.conc for e in combatant.effects):
		effects.append('concentrating')
	if any(IsBuf(e) for e in combatant.effects):
		effects.append('buffed')
	if any(IsDebuf(e) for e in combatant.effects):
		effects.append('debuffed')
	return effects

def CollectTargets(args):
	if True in args or 'True' in args or 'all' in args:
		return [c for c in combat().combatants if c.type=='combatant' and c.note and 'Location' in c.note]
	else:
		split_args=[n.strip() for arg in args for n in arg.split(',')]
		combatants = [combat().get_combatant(n) or combat().get_group(n) for n in split_args]
		result=[]
		for c in combatants:
			if not c:
				continue
			if c.type=='group':
				result+=c.combatants
			elif c.type=='combatant':
				result.append(c)
		return result

def ParseTime(time_str):
	for postfix, mul in dict(s=1, sec=1, m=60, min=60, h=3600, hr=3600, hour=3600).items():
		if time_str.endswith(postfix):
			return int(time_str[:-len(postfix)])*mul
	return int(time_str)

# parse arguments
args=&ARGS&
nl = '\n'
if not args:
	effects=set()
	for c in C.combatants:
		effects.update(CombatantEffects(c)[:1])
	used_colors=dict()
	for condition, color_name in conditions.items():
		if condition in effects:
			used_colors[color_name]=used_colors.get(color_name,[])+[condition]
	legend=[f'{colors[color_name].icon} - {", ".join(conditions)}' for color_name, conditions in used_colors.items()]
	# TODO report only active conditions and the combatants afflicted
	#legend=[f'{color.icon} - {", ".join(condition_name for condition_name, condition_color in conditions.items() if condition_color==color_name)}' for color_name,color in colors.items()]
	return f'''embed -title "color legend" -desc "{nl.join(legend)}" '''
else:
	desc=[]
	pargs = argparse(args)
	if timer:=pargs.last('t'):
		timer=ParseTime(timer)
		timer_arg=f'-t {timer}'
	else:
		timer_arg=''
	overlay_nr = pargs.last('nr',8,type_=int)
	applied=dict()

	# collect all explicit colored arguments
	for color_name, color in colors.items():
		for c in CollectTargets(pargs.get(color_name)+pargs.get(color.tag)):
			applied[c] = color_name
			desc.append(f'{color.icon} {c.name}')

	# auto color all specified targets that are not already specified
	for c in CollectTargets(pargs.get('auto')):
		if any(c.name in a.name for a in applied):
			continue
		effects = CombatantEffects(c)
		for condition, color in conditions.items():
			if condition in effects:
				applied[c]=color
				desc.append(f'{colors[color].icon} {c.name} [{condition}]')
				break	# conditions are sorted by priority

	# Apply all the colors to the overlay
	for c, color_name in applied.items():
		color=colors[color_name]
		if not c:
			continue
		notes = ParseMapNotes(c.note)
		# get the size of the creature
		size = notes.get('Size','M')[0]
		# find the appropriate circle type (corner or not) and radius based on the size
		prefix = ring_prefix[size]
		overlay=f'Overlay{overlay_nr}'
		if color.tag:
			notes[overlay]=f'u{prefix}{color.tag}'+'{targ}'
		elif overlay in notes:
			notes.pop(overlay)
		c.set_note(SerializeMapNotes(notes))
	if not desc:
		desc=['None']
	return f'''embed -title "Colors applied" -desc "{nl.join(desc)}" {timer_arg}'''
</drac2>