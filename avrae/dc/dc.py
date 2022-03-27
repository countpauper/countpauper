<drac2>
var_name='dc_db'
db=load_yaml(get_gvar(get_svar(var_name,'5a2806d6-b3d1-4253-a4fe-fb090910e896')))
# return f'echo ```{db}```'
syntax=f'{ctx.prefix}{ctx.alias} <check_type> <base> ["<modifier>"|-<modifier> "<value>"|"<value> <modifier>"]...'
if (args:=&ARGS&):
	check_str=args.pop(0).lower()
	check_type=([chk for chk in db if chk.lower().startswith(check_str)]+[None])[0]
else:
	check_str, check_type='?', None
if check_str in "?help" or not check_type:
	return f'echo `{syntax}`.\nCheck type can be one of {", ".join(db)}.'

units=dict(
	foot=1,
	ft=1,
	inch=1/12,
	mile=5280,
	second=1/60,
	minute=1,
	min=1,
	hour=60,
	hr=60,
	day=24*60,
	week=7*24*60)

available_modifiers=db[check_type]
skill_key='skill'
if skill:=available_modifiers.get(skill_key):
	available_modifiers.pop(skill_key)

# convert dict keys with units to base amounts
for mod, val in available_modifiers.items():
	if typeof(val)=='SafeDict':
		remove_list=[]
		new_vals=dict()
		for k,v in val.items():
			if typeof(k)=='str' and (unit:=([u for u in units if k.lower().endswith(u)]+[None])[0]):
				k_amount=k.replace(' ','')[:-len(unit)]
				if k_amount.isdecimal():
					new_vals[int(k_amount)*units[unit]] = v
					remove_list.append(k)
				elif not k_amount:
					new_vals[units[unit]] = v
					remove_list.append(k)
		val.update(new_vals)
		available_modifiers[mod]={k:v for k,v in val.items() if k not in remove_list}

# parse arguments into a dictionary key:val where -a b will be added a:b just a will be added as a:None
arg_dict=dict()
while args:
	arg=args.pop(0).lower()
	if arg.startswith('-') and args and len(arg)>1:
		argval=args.pop(0)
		arg_dict[arg[1:]]= argval
	elif endmatches:=[mod for mod in available_modifiers if arg.endswith(' '+mod.lower())]:
		arg_dict[endmatches[0].lower()]=arg[:-len(endmatches[0])-1]
	else:
		arg_dict[arg]=None

# convert argument values that are decimal with or without unit
for arg, val in arg_dict.items():
	# TODO: convert unit arguments
	if val is None:
		continue
	if val.isdecimal():
		arg_dict[arg]=int(val)
	elif unit:=([u for u in units if val.endswith(u)]+[None])[0]:
		unit_val=val.replace(' ','')[:-len(unit)]
		if unit_val.isdecimal():
			arg_dict[arg]=int(unit_val)*units[unit]
		elif not unit_val:
			arg_dict[arg]=units[unit]

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
			elif typeof(val)=='SafeDict':
				if typeof(argval)=='int':
					best=dict(key=None, delta=2**31)
					for k,v in val.items():
						if typeof(k)=='int':
							if (delta:=abs(k - argval))<best.delta:
								best=dict(key=k, delta=delta)
					if best.key is not None:
						if best.delta:
							modifiers[f'{mod}≈{best.key}'] = val[best.key]
						else:
							modifiers[f'{mod}={argval}'] = val[best.key]
						available_modifiers.pop(mod)
						break
				elif argval and (key_match:={k:v for k,v in val.items() if str(k).startswith(argval)}):
					first=list(key_match.keys())[0]
					modifiers[f'{mod}={first}'] = key_match[first]
					available_modifiers.pop(mod)
					break
				return f'echo For `{argkey}` provide one of the following values: {", ".join(str(v) for v in val)}'

	else:
		if argval is None:
			# go over all dictionary modifiers again to partial match the argument with all values
			for mod, val in available_modifiers.items():
				if typeof(val)=='SafeDict':
					# matches with small or small size (or 3/4 cover)
					if matches:=[k for k in val if f'{k} {mod}'.lower().startswith(argkey)]:
						match=matches[0]
						modifiers[f'{mod}={match}']=val[match]
						available_modifiers.pop(mod)
						break
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
	return f'echo `{syntax}`. Select at least one DC modifier from {", ".join(available_modifiers)}.'
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
