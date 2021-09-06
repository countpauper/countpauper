<drac2>
cc='Ki Points'
ch=character()
args=argparse(&ARGS&)
ignore=args.last('i',False)
MonkLevel = int(get('MonkLevel', 0))
default_dc=8 + proficiencyBonus + wisdomMod
field=f'-desc "When you hit another creature with a melee weapon attack, you can spend 1 ki point to attempt a Stunning Strike. The target must succeed on a Constitution saving throw [DC {default_dc}] or be Stunned until the end of your next turn."'

# Counter
target_args=[t.split('|') for t in args.get('t')]
if not ignore:
	ki = max(1, len(target_args))
	if not ch.cc_exists(cc):
		err(f'You don\'t have {cc}, use `!level Monk {MonkLevel}`')
	if ch.get_cc(cc)<ki:
		err(f'You don\'t have {ki} {cc} left. You need to rest first.')

	ch.mod_cc(cc,-ki)
	field+=f' -f "{cc} [-{ki}]|{ch.cc_str(cc)}"'
else:
	field+=f' -f "{cc}|Not used"'

c=combat()
me = c.me if c else None
if me and target_args:
	combatants={t[0]:c.get_combatant(t[0]) for t in target_args if t}
	missing=[t for t,c in combatants.items() if c is None]
	if missing:
		err(f'The following Stunning Blow targets were not found : `{",".join(missing)}`')

	# parse target contextual arguments
	[args.add_context(ta[0], argparse(" ".join(ta[1:]))) for ta in target_args]
	for target in combatants:
		args.set_context(target)
		combatant = combatants[target]
		if args.last('fail'):
			save='`fail`'
			stunned=True
		elif args.last('pass'):
			save='`pass`'
			stunned=False
		else:
			dc = args.last('dc', default_dc)
			save_roll='+'.join([combatant.saves.get('con').d20(args.adv(boolwise=True))]+args.get('b'))
			save = vroll(save_roll)
			stunned = (save.total < dc)
		if stunned:
			dur=2 if c.current and c.current.name == c.me.name else 1
			en = f'Stunning Strike ({combatant.name})'
			me.add_effect(en, '', duration=dur, end=True, desc='Targets are stunned until the end of your next turn.') 	# stacking=True
			parent = me.get_effect(en)
			combatant.add_effect('Stunned','', parent=parent, desc="""A stunned creature is incapacitated (can't act or react), can’t move, and can speak only falteringly.
The creature automatically fails Strength and Dexterity Saving Throws.
Attack Rolls against the creature have advantage.""")
		field+=f' -f "{combatant.name}|{save} : {"Stunned!" if stunned else "Resisted!"}|inline"'

return f'embed -title "{name} Strikes Stunningly!" {field} -thumb {image}'
</drac2>
