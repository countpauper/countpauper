!alias max_hp <drac2>
argstr="&*&"
if not argstr:
	return f'echo {ctx.prefix}{ctx.alias} [+|-]maxhp [-t <target>][...]'
mhp=argstr.split()[0]
args=argparse(argstr)
if c:=combat():
	targets=[c.get_combatant(t) for t in args.get('t')] or [c.me]
	if mhp[0]=='+':
		hp_delta=int(mhp[1:])
		hp_set=None
	elif mhp[0]=='-':
		hp_delta=int(mhp)
		hp_set=None
	elif mhp=='reset':
		targets=[c.me]
		hp_delta=0
		hp_set=character().max_hp
	else:
		hp_delta=0
		hp_set=int(mhp)

	fields=[]
	for target in targets:
		if target is None:
			continue
		if hp_set is None:
			current_max=character().max_hp if target.name==c.me.name else target.max_hp
			target.set_maxhp(current_max+hp_delta)
		else:
			target.set_maxhp(hp_set)
		fields.append(f'-f "{target.name}|{target.hp_str()}|inline"')
	if hp_set:
		title = f'Maximum HP set to {hp_set}'
	else:
		title = f'Maximum HP changed by {hp_delta}.'
	return f'embed -title "{title}" {" ".join(fields)}'
else:
	return f'echo Maximum hp can only be changed in iniative.'
</drac2>