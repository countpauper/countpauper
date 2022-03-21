<drac2>
var_name='dc_db'
db=load_yaml(get_gvar(get_svar(var_name,'5a2806d6-b3d1-4253-a4fe-fb090910e896')))
# return f'echo ```{db}```'
syntax=f'{ctx.prefix}{ctx.alias} <check_type> <main modifier> [<modifiers>|<modifier>=<value>|-<modifier> <value>]...'
if not (args:=&ARGS&):
	return f'echo `{syntax}`'
check_str=args.pop(0).lower()
check_type=([chk for chk in db if check_str.startswith(chk.lower())]+[None])[0]
if check_str in "?help" or not check_type:
	return f'echo `{syntax}`.\nCheck type can be one of {", ".join(db)}.'
available_modifiers=db[check_type]

# parse arguments into a dictionary key:val where -a b will be added a:b just a will be added as a:None
arg_dict=dict()
while args:
	arg=args.pop(0).lower()
	if arg.startswith('-') and args and len(arg)>1:
		argval=args.pop(0)
		arg_dict[arg[1:]]=int(argval) if argval and argval.isdecimal() else argval
	else:
		arg_dict[arg]=None

modifiers=dict()
unhandled_args=[]
while arg_dict:
	argkey=list(arg_dict.keys())[0]
	argval=arg_dict.pop(argkey)

	for mod, val in available_modifiers.items():
		if mod.lower().startswith(argkey):
			if typeof(val)=='int':
				modifiers[mod]=val
				break
			elif typeof(val)=='bool':
				modifiers[mod]=val
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
						break
				elif argval and (key_match:={k:v for k,v in val.items() if k.startswith(argval)}):
					first=list(key_match.keys())[0]
					modifiers[f'{mod}={first}'] = key_match[first]
					break
				return f'echo For `{argkey}` provide one of the following values: {", ".join(str(v) for v in val)}'
	else:
		# go over all modifiers again to partial match the argument with all values of dictionary modifiers
		for mod, val in available_modifiers.items():
			if typeof(val)=='SafeDict':
				if matches:=[k for k in val if str(k).lower().startswith(argkey)]:
					match=matches[0]
					modifiers[f'{mod}={match}']=val[match]
					break
		else:
			unhandled_args.append(argkey)

if unhandled_args:
	return f'echo Unhandled arguments: {", ".join(f"`{a}`" for a in unhandled_args)}'
if not modifiers:
	return f'echo `{syntax}`. Select at least one DC modifier from {", ".join(available_modifiers)}.'
if fail:=any(bonus is False for bonus in modifiers.values()):
	return f'echo check_type {" ".join(selected_modifiers)} = fail'
elif success:=any(bonus is True for bonus in modifiers.values()):
	return f'echo check_type {" ".join(selected_modifiers)} = success'
dc_roll='+'.join(f'{bonus}[{mod}]' for mod,bonus in modifiers.items()).replace('+-','-')
return f'echo {vroll(dc_roll)}'
</drac2>
