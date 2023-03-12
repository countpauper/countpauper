<drac2>
args=&ARGS&
def is_dice(s):
	return all(a in "0123456789d+-*/() " for a in s)

# parse arguments
delta=None
target = None
if args:
	if is_dice(args[0]):
		delta=args.pop(0)
	else:
		target=args.pop(0)
		if args:
			delta=args.pop(0)
if message:=" ".join(args):
	message=f' *{message}*'

def negate_roll_str(s):
	if s[0]=='-':
		if any(c in s[1:] for c in '+-'):
			return f'-({s})'
		else:
			return s[1:]
	else:
		return f'-({s})'

# execute for combat
if c:=combat():
	# find combat target
	if target:
		target = c.get_combatant(target)
	else:
		target = c.me or c.current
	if not target:
		return f'echo No combatant found.'

	# apply delta
	if delta:
		if delta[0]=='-':
			r=target.damage(negate_roll_str(delta)).roll
			return f'echo hurts {target.name} by {r}. {target.hp_str()}{message}'
		else:
			r=vroll(delta)
			target.modify_hp(r.total, False, False)
			return f'echo heals {target.name} by {r}. {target.hp_str()}{message}'
	# or just output the current hitpoints
	else:
			return f'i hp {target.name}'
# execute out of combat
else:
	if target is None or name.lower().startswith(target.lower()):
		if delta:
			if delta[0] in '+-':
				return f'g hp {delta}'
			else:
				return f'g hp +{delta}'
		else:
			return f'g hp'
	else:
		return f'echo You can not {"inspect" if delta is None else "change"} the hitpoints of {target} outside of initiatve.'
</drac2>