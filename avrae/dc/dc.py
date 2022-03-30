<drac2>
var_name='dc_db'
data = load_yaml(get_svar(var_name,get(var_name,'5a2806d6-b3d1-4253-a4fe-fb090910e896')))
if typeof(data)!='SafeList':
	data=[data]
db=dict()
for d in data:
	if (typeof(d)=='str') and len(d)==36:
		db.update(load_yaml(get_gvar(d)))
	elif typeof(d)=='SafeDict':
		db.update(d)
	else:
		return f'echo Unrecognized data `{d}` in `{var_name}`. Use a (list of) `{ctx.prefix}gvar` guids referencing yaml data or direct yaml.'

# return f'echo ```{db}```'
syntax=f'{ctx.prefix}{ctx.alias} <check_type> [<sub type>]... <base> ["<modifier>"|-<modifier> "<value>"|"<value> <modifier>"]...'

args, check_dir=&ARGS&, []
skill_key, skill='skill', None
while args:
	check_str=args.pop(0).lower()
	if check_type:=([chk for chk in db if chk.lower().startswith(check_str)]+[None])[0]:
		check_dir.append(check_type)
		available_modifiers=db[check_type]
		if skill:=available_modifiers.get(skill_key):
			available_modifiers.pop(skill_key)
			break
		else:
			db=db[check_type]
	elif check_str in ['?','help']:
		return f'echo `{syntax}`.\n{" ".join(check_dir) or "Check"} can be one of {", ".join(db)}.'
	else:
		return f'echo `{check_str}` is not recognized. Choose one of {", ".join(db)}.'
if not skill:
	return f'echo `{syntax}`.\n{" ".join(check_dir) or "Check"} can be one of {", ".join(db)}.'

if not check_dir:
	return f'echo `{syntax}`.\nCheck can be one of {", ".join(db)}.'

units=table=load_yaml(get_gvar('7b9457fc-3dfa-44f6-a0a2-794388ca9a28'))

# parse remaining arguments into a dictionary key:val where -a b will be added a:b just a will be added as a:None
arg_dict=dict()
while args:
	arg=args.pop(0).lower()
	if arg.startswith('-') and args and len(arg)>1:
		argval=args.pop(0)
		arg_dict[arg[1:]]= argval
	elif endmatches:=[mod for mod in available_modifiers if arg.endswith(' '+mod.lower())]:
		arg_dict[endmatches[0].lower()]=arg[:-len(endmatches[0])-1].lower()
	else:
		arg_dict[arg]=None

# return f'echo ```{available_modifiers}\n{remove_list}\n{arg_dict}```'
modifiers=dict()
unhandled_args=[]
while arg_dict:
	argkey=list(arg_dict.keys())[0]
	argval=arg_dict.pop(argkey)
	for mod, val in available_modifiers.items():
		if mod.lower().startswith(argkey):
			if typeof(val)=='int':
				modifiers[mod]=val
				available_modifiers.pop(mod)
				break
			elif  typeof(val)=='bool':
				modifiers[mod]=val
				available_modifiers.pop(mod)
				break
		if typeof(val)=='SafeDict' and (argval is None or mod.lower().startswith(argkey)):
			search_key = argval or argkey
			# find the unit if the argument and compatible conversions
			arg_conversion=dict()
			if arg_unit:=([unit for unit in units if str(search_key).lower().endswith(unit)]+[None])[0]:
				arg_value = search_key.replace(' ', '')[:-len(arg_unit)]
				if not arg_value.isalpha():
					search_key=roll(arg_value)	# assume it's 2d6*4<unit>, but
					arg_conversion=units[arg_unit]
					arg_conversion[arg_unit]=1
			elif typeof(search_key)=='str' and not search_key.isalpha():
				search_key=roll(search_key)

			if typeof(search_key)=='int':
				dbg=dict()
				best=dict(key=None, delta=2**31)
				for k,v in val.items():
					if typeof(k) == 'str' and (key_unit := ([unit for unit in arg_conversion if k.lower().endswith(unit)] + [None])[0]):
						if key_value:=k.replace(' ', '')[:-len(key_unit)]:
							if key_value.isdecimal():
								key_value=int(key_value)
							elif key_value.replace('.','').isdecimal():
								key_value=float(key_value)
							else:
								continue
						else:
							key_value=1
						# dbg[k] = f'{key_value}/{arg_conversion[key_unit]} {key_unit}'
						converted_value=float(key_value) / arg_conversion[key_unit]
						if (delta := abs(converted_value - search_key)) < best.delta:
							best = dict(key=f'{search_key} {arg_unit}', delta=delta, val=v)
					elif typeof(k)=='int':
						if (delta:=abs(k - search_key))<best.delta:
							best=dict(key=k, delta=delta, val=v)
				# return f'echo {best} with `{dbg}` using `{arg_conversion}`'
				if best.key is not None:
					if best.delta:
						modifiers[f'{mod}≈{best.key}'] = best.val
					else:
						modifiers[f'{mod}={best.key}'] = best.val
					available_modifiers.pop(mod)
					break
			if key_match:={k:v for k,v in val.items() if str(k).startswith(str(search_key))}:
				first=list(key_match.keys())[0]
				modifiers[f'{mod}={first}'] = key_match[first]
				available_modifiers.pop(mod)
				break
			if argval:
				return f'echo For `{argkey}` provide one of the following values: {", ".join(str(v) for v in val)}'
	else:
		unhandled_args.append(argkey)
if unhandled_args:
	return f'echo Unhandled arguments: {", ".join(f"`{a}`" for a in unhandled_args)}'
rollargs=[skill] if skill else []
# collect all advantage and disadvantage bonuses and filter them out
adv=[mod for mod, bonus in modifiers.items() if bonus=='adv']
dis=[mod for mod, bonus in modifiers.items() if bonus=='dis']
if adv and dis:
	advdis=f'straight because {", ".join(adv)}, but {", ".join(dis)}'
elif adv:
	advdis=f'with advantage because {", ".join(adv)}'
	rollargs.append('adv')
elif dis:
	advdis=f'with disadvantage because {", ".join(dis)}'
	rollargs.append('dis')
else:
	advdis=""
modifiers={mod:bonus for mod, bonus in modifiers.items() if mod not in adv+dis}
if not modifiers:
	return f'echo `{syntax}`. Select at least one {skill} DC modifier from {", ".join(available_modifiers)}.'
if fail:=[mod for mod, bonus in modifiers.items() if bonus is False]:
	dc=f'fail because {", ".join(fail)}'
	return f'echo {skill + " " if skill else ""}{dc} {advdis}'
elif success:=[mod for mod, bonus in modifiers.items() if bonus is True]:
	dc=f'pass because {", ".join(success)}'
	return f'echo {skill + " " if skill else ""}{dc} {advdis}'
else:
	dc=vroll('+'.join(f'{bonus}[{mod}]' for mod,bonus in modifiers.items()).replace('+-','-'))
	rollargs.append(f'-dc {dc.total}')
skill=f'{skill} ' if skill else ''
unspecified=[]
for mod, val in available_modifiers.items():
	if typeof(val)=='SafeDict':
		if zero_val:=([k for k,v in val.items() if v is 0]+[None])[0]:
			unspecified.append(f'{mod}={zero_val}')
		else:
			unspecified.append(f'{mod}=unknown')
	else:
		unspecified.append(f'not {mod}')
unspecified=f'\nAssumed: {", ".join(unspecified)}' if unspecified else ""
return f'''echo {skill}{dc} {advdis}{unspecified}
`{ctx.prefix}check {" ".join(rollargs)}`'''
</drac2>
