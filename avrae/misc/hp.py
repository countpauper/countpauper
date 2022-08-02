!alias hp <drac2>
args=&ARGS&
if '?' in args or 'help' in args:
	return f'echo `{ctx.prefix}{ctx.alias} [<target>=you] [[-]<hp modification> [<reason with spaces, no quotes needed>]]`'

if c:=combat():
	if not args:
		if t:=c.current:
			return f'i hp {t.name}'
		elif c.me:
			return f'i hp {c.me.name}'
		else:
			return f'echo No target found.'
	elif len(args)==1:
		return f'i hp {args[0]}'
	else:
		target=c.get_combatant(args.pop(0))
		delta=args.pop(0)
else:
	if not args:
		return 'g hp'
	else:
		target = character()
		first = args.pop(0)
		if first.strip('+-').isdigit():
			delta=int(first)
		elif name.lower().startswith(first.lower()):
			if args:
				delta=int(args[0])
			else:
				return 'g hp'
		else:
			return f'echo You can not {"change" if args else "inspect"} the hitpoints of {first} outside of initiatve.'
		if delta>0:
			return f'g hp +{delta}'
		else:
			return f'g hp {delta}'

if not target:
	return f'echo Target not found.'

if message:=" ".join(args):
	message=f' *{message}*'
if delta[0]=='-':
	delta=delta[1:]
	if c:
		r=target.damage(delta).roll
	else:
		r=vroll(delta)
		target.modify_hp(-r.total, False, False)
	return f'echo hurts {target.name} by {r}. {target.hp_str()}{message}'
else:
	r=vroll(delta)
	target.modify_hp(r.total, False, False)
	return f'echo heals {target.name} by {r}. {target.hp_str()}{message}'
</drac2>