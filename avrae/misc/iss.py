<drac2>
# !iss <target> [level] [value] [args...]

def isinteger(s):
	if s[0] in '+-':
		s=s[1:]
	return s and s.isdecimal()

### Parse the arguments
args=&ARGS&
# No arguments
if not args:
	return f'echo `{ctx.prefix}{ctx.alias} <target> [level] [+/-][value] [nopact] [book]`'
# First argument: target
C=combat()
target_arg=args.pop(0).lower()
if not C:
	if name.lower().startswith(target_arg):
		return f'g ss {" ".join(args)}'
	else:
		return f'''echo Unable to access {target_arg.capitalize()}'s spellslots when not in initiative.'''
if target:=C.get_combatant(target_arg):
	pass
elif name.lower().startswith(target_arg):
	target=character()
else:
	return f'echo Unable to find the target `{target_arg}` in the current initiative list.'

# second argument: level
if args and args[0].isdecimal():
	level=int(args.pop(0))
	if level<=0:
		return f'echo `{level} is an invalid spell level.'
	header=f'__**Remaining Level {level} Spell Slots**__'
	levels=[level]

	# third argument delta:
	modifier = args.pop(0) if args and isinteger(args[0]) else None
else:
	header=f'__**Remaining Spell Slots**__'
	levels=range(1,10)
	modifier = None

# Remaining arguments, argparse
args=argparse(args)

# Modify the spell slots
if not (sb:=target.spellbook):
	return f'echo Target `{target_arg}` is not a spellcaster.'

if modifier:
	current_slots = sb.get_slots(level)
	if modifier[0] == '-':
		delta = -int(modifier[1:])
	elif modifier[0] == '+':
		delta = int(modifier[1:])
	else:
		delta = min(sb.get_max_slots(level), int(modifier)) - current_slots
	sb.set_slots(level, current_slots + delta, pact=not args.last('nopact'))
else:
	delta=None

# Format the output embed
book=''
if args.last('book'):
	book=f'-f "Spellbook|{", ".join(s.name for s in sb.spells if s.prepared)}"'
nl='\n'
slots=[]
for lvl in levels:
	if sb.get_max_slots(lvl):
		slots.append(f'{sb.slots_str(lvl)}' + ('' if delta is None else f' ({delta})'))
if not slots:
	return f'embed -title "{target.name}" -desc "{header}\nNone." {book}'
else:
	return f'embed -title "{target.name}" -desc "{header}\n{nl.join(slots)}" {book} -footer "◉ = Available / 〇 = Used"'
</drac2>
