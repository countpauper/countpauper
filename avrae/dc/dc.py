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
remaining_args=[]
modifiers=dict()
key_val_seperator='='
while args:
	arg=args.pop()
	if key_val_seperator in arg:
		argkey, argval = arg.split(key_val_seperator, maxsplit=1)
		if argval.isdecimal():
			argval=int(argval)
	else:
		argkey=arg
		argval=None

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
					pass # TODO: find nearest of integer val.val
				else:
					if argval and (key_match:={k:v for k,v in val.items() if k.startswith(argval)}):
						first=list(key_match.keys())[0]
						modifiers[f'{mod}={first}'] = key_match[first]
						break
					else:
						return f'echo For `{argkey}` provide one of the following values: {", ".join(val)}'

			# TODO: if type of val is a dict, then split the argument, compare the key and find the value. If the value is decimal use nearest
			# Add modifier also as mod=value or mod≈value
	else:
		remaining_args.append(arg)
# TODO: pass once more over remaining_args to look for value mods
if remaining_args:
	return f'echo Unhandled arguments: {", ".join(f"`{a}`" for a in remaining_args)}'
if not modifiers:
	return f'echo `{syntax}`. Select at least one DC modifier from {", ".join(available_modifiers)}.'
if fail:=any(bonus is False for bonus in modifiers.values()):
	return f'echo check_type {" ".join(selected_modifiers)} = fail'
elif success:=any(bonus is True for bonus in modifiers.values()):
	return f'echo check_type {" ".join(selected_modifiers)} = success'
dc=sum(int(bonus) for bonus in modifiers.values())
# TODO: format formula (and roll it, support 1d6 values) but remove +-
return f'echo {check_type} `{"+".join(f"{bonus}[{mod}]" for mod,bonus in modifiers.items())}` = {dc}'
</drac2>
