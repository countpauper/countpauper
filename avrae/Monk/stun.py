<drac2>
cc='Ki Points'
ch=character()
args=argparse(&ARGS&)
ignore=args.last('i',False)
MonkLevel = int(get('MonkLevel', 0))
dc = args.last('dc', 8 + proficiencyBonus + wisdomMod)

field=f'-desc "When you hit another creature with a melee weapon Attack, you can spend 1 ki point to attempt a Stunning Strike. The target must succeed on a Constitution saving throw [DC {dc}] or be Stunned until the end of your next turn."'

# Counter
targets=args.get('t')
if not ignore:
	ki = max(1, len(targets))
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
if me and targets:
	targets=[t.split('|') for t in targets]
	targets={t[0]:t[1:] for t in targets if t}
	combatants={t:c.get_combatant(t) for t in targets.keys()}
	missing=[t for t,c in combatants.items() if c is None]
	if missing:
		err(f'The following Stunning Blow targets were not found : `{",".join(missing)}`')

	en = 'Stunning Strike'
	parent = me.get_effect(en)
	if not parent:
		parent=me.add_effect(en,'',duration=2, end=True,desc='Targets are stunned until the end of your next turn.')
	for target, extra in targets.items():
		target_args=argparse(&ARGS&+extra)
		combatant = combatants[target]
		# TODO: to use -b, it would have to manually make the roll string with AliasSaves.get('wisdom').d20(...)+bonus
		save = combatant.save('con',target_args.adv(boolwise=True))
		stunned = False
		if save.total<dc:
			stunned = True
			combatant.add_effect('Stunned','', parent=parent, desc="""A stunned creature is incapacitated (can't act or react), can’t move, and can speak only falteringly.
The creature automatically fails Strength and Dexterity Saving Throws.
Attack Rolls against the creature have advantage.""")
		field+=f' -f "{combatant.name}|{save} : {"Failure!" if stunned else "Success!"}|inline"'

return f'embed -title "{name} Strikes Stunningly!" {field} -thumb {image}'
</drac2>
