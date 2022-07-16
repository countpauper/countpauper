<drac2>
c=combat()
cc = 'Channel Divinity'
sanctuary_effect = 'Twilight Sanctuary'
action = 'Channel Divinity: Twilight Sanctuary'
if not c:
	return f'echo `{ctx.prefix}{ctx.alias}` only works in combat. If you want to activate it before combat, use `!a "{action}"` or `!cc "{cc}" -1`'

args=argparse('''&*&''')
clerics=[cleric for cleric in c.combatants if cleric.levels.get('Cleric')>=2]
twilight_clerics=[cleric for cleric in clerics if cleric.get_effect('Twilight Sanctuary')]
config=load_json(get_svar('twilight_sanctuary','{}'))
formula=config.get('formula','ClericLevel+1d6')
radius=int(config.get('radius',30))
formula_descriptions={
	'ClericLevel':'your cleric level',
	'level':'your character level',
	'+':' plus ',
	'proficiencyBonus': 'your proficiency bonus',
	'wisdomMod':'your wisdom bonus'}
formula_desc = formula
for k,d in formula_descriptions.items():
	formula_desc = formula_desc.replace(k,d)

if not twilight_clerics:
	# no twilight clerics, try to use it instead
	if (me:=c.me) and (C:=character()) and C.levels.get('Cleric')>=2:
		if C.cc_exists(cc) and C.get_cc(cc)>0:
			# Add the overlay to the notes
			overlay_nr = args.last('under',args.last('over',2, type_=int), type_=int)
			color=args.last('color',args.last('c','~202080'))
			# get notes in a list
			note = me.note or ""
			notes = [n.strip() for n in note.split('|') if n]
			# remove old overlay notes
			notes = [n for n in notes if not any(prefix in n for prefix in (f'Overlay{overlay_nr}', f'Effect{overlay_nr}', f'Aim{overlay_nr}'))]
			# add new overlay notes
			notes.append(f'Overlay{overlay_nr}: uc{radius}{color}'+'{targ}')
			notes.append(f'Effect{overlay_nr}: {sanctuary_effect} / {me.name}')
			me.set_note(' | '.join(notes))
			# Execute the action
# can't detect if licensed		if any(a.name==action for a in C.actions):
#				return f'a "{action}"'
#			else:
			C.mod_cc(cc,-1)
			c.me.add_effect(sanctuary_effect,10)
			return f"""embed -title "{C.name} uses {action}" -f "{cc}|{C.get_cc(cc)}" -desc "As an action, you present your holy symbol, and a sphere of twilight emanates from you. The sphere is centered on you, has a {radius}-foot radius, and is filled with dim light. The sphere moves with you, and it lasts for 1 minute or until you are incapacitated or die. Whenever a creature (including you) ends its turn in the sphere, you can grant that creature one of these benefits:
- You grant it temporary hit points equal to {formula_desc}.
- You end one effect on it causing it to 	be charmed or frightened." -thumb {image}"""
		else:
			return f'echo You are currently unable to use your {action} because you have no {cc} available.'
	else:
		return f'echo You can not get the benefits, because no clerics with an active Twilight Sanctuary found.'

# twilight cleric found, apply the effect
cleric = twilight_clerics[0]  # TODO: nearest and within range or highest level in range

if target_args:=args.get('t'):
	target_args=[ta.split('|') for ta in target_args]
	target_args={c.get_combatant(ta[0]):ta[1:] for ta in target_args}
	for tt, ta in target_args.items():
		if tt:
			args.add_context(tt.name, argparse(ta))
	targets=[t for t in target_args.keys() if t]
elif c.me:
	targets=[c.me]
elif c.current:
	targets=[c.current]
else:
	return f'echo No active combatant or specified target. Use `{ctx.prefix}{ctx.alias} -t <target>`'

# convert the variables in the formula for the found cleric
for (cls, lvl) in cleric.levels:
	formula = formula.replace(f'{cls}Level', str(lvl))
formula = formula.replace('level', str(cleric.levels.total_level))
formula = formula.replace('profiencyBonus', str(cleric.stats.prof_bonus))
for s in ('Strength', 'Dexterity', 'Constitution', 'Intelligence', 'Wisdom', 'Charisma'):
	formula = formula.replace(f'{s.lower()}Mod', str(cleric.stats.get_mod(s)))

# apply and describe the results to all targets
results=[]
for t in targets:
	args.set_context(t.name)
	removable_effects=dict(charm='charmed',fear='frightened')
	commands=['thp'] if args.get('thp') else []
	[commands.append(effect_command) for effect_command in removable_effects.keys() if args.get(effect_command)]
	if commands:
		# explicit command argument (todo: add target context)
		command=commands[0]
	else:
		# auto resolve command
		for cmd, effect in removable_effects.items():
			if t.get_effect(effect):
				command=cmd
				break
		else:
			command = 'thp'

	# return f'echo {commands} {command}'

	if to_remove:=removable_effects.get(command):
		if e := t.get_effect(to_remove):
			t.remove_effect(e)
			results.append(f"Removed {e.name} from {t.name}")
		else:
			results.append(f"Failed to remove {to_remove} from {t.name} since they are not affected.")
	else:
		new_thp=vroll(formula)
		thp = t.temp_hp
		if thp<=new_thp.total:
			t.set_temp_hp(new_thp.total)
			results.append(f"Gave {t.name} {new_thp} temporary hitpoints {t.hp_str()}")
		else:
			results.append(f"Kept {t.name}'s temporary hitpoints {t.hp_str()}, because it's more than {new_thp}")
nl='\n'
return f'''embed -title "{cleric.name}'s Twilight sanctuary" -desc "{nl.join(results)}" -thumb https://cdn.discordapp.com/attachments/751105864370356365/934550087659565097/Twilight.png'''
</drac2>