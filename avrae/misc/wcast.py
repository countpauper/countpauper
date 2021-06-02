<drac2>
# Data
spell_gvar  = load_json(get_gvar('5e2c8e11-6ade-4ce2-9ece-562fc9db70c7'))
meta_gvar   = load_json(get_gvar('9af34023-6eba-4789-82b1-e68b191220ac'))
cc          = "Sorcery Points"
footer='-f "Made by Croebh, Modified by Velglarn"'

# Arguments
args        = &ARGS&

# If not provided, search through the gvar for that spell
spell_name  = args[0] if args else ''
spells=[s for s in spell_gvar if spell_name.lower()==s.name.lower()]
if not spells:
	spells=[s for s in spell_gvar if spell_name.lower() in s.name.lower()]
if not spells:
	return f"""embed -title "Spell not found!" -desc "Sorry, `{spell_name}` could not be matched to a level in the database. If this is a valid spell (perhaps homebrew?) you can bypass this by specifying a level with `-l #`" """
spell_name=spells[0].name
spell_level=spells[0].level
spell_level = argparse(args).last('l', spell_level, type_=int)

# Figure out which meta magics we're using
field_out  = []
meta_cost = 0
ch=character()
for meta in meta_gvar:
	if any([x for x in meta.args if x in args]):
		field_out.append(meta.out)
		# Min 1 for cantrips
		meta_cost += max(int(get(meta.points, meta.points)),1)

if not ch.cc_exists(cc) and meta_cost:
	return f"""embed -title "I'm sorry {ctx.author.display_name}, I can't let you do that." -desc "You are missing a Sorcery Points custom counter. You can either create one yourself (`!help cc create`), or use the [`{ctx.prefix}level` alias](https://avrae.io/dashboard/workshop/5f7385fe647bb0a416316d1d)." """
elif meta_cost:
	if ch.cc_exists(cc) and character().get_cc(cc) >= meta_cost:
		ch.mod_cc(cc, -meta_cost)
	else:
		return f"""embed -title "I'm sorry {ctx.author.display_name}, I can't let you do that." -desc "You don't have enough Sorcery Points for that." """

field_out.append(f""" -f "{cc} {f'(-{meta_cost})' if meta_cost and ch.get_cc(cc) >= meta_cost else ''}|{ch.cc_str(cc) if character().cc_exists(cc) else '*None*'}|inline"  """)

# If no arguments, return the help
if not args:
	return f"help {ctx.alias} -here"

wild='wild' in load_json(get('subclass','{}')).get('SorcererLevel','').lower()
# build in wild magic. it's once per turn but on leveled spells which (baring action surge) are once per turn at most
if wild and spell_level>0:
	surge_db = load_json(get_gvar('9274bf9a-f3d2-407e-9723-0a8901d04003'))
	tidesCC = "Tides of Chaos"

	tides=ch.cc_exists(tidesCC) and ch.get_cc(tidesCC)<ch.get_cc_max(tidesCC)
	if surge_roll:= argparse(args).last('surge'):
		if not '-surge' in args:
			surge_roll='1d100'
		chanceRoll=''
	elif tides:
		chanceRoll=tidesCC
		surge_roll='1d100'
	else:
		chanceRoll=vroll('1d20')
		if chanceRoll.total==1:
			surge_roll='1d100'
		else:
			field_out.append(f'-f "Wild Surge|{chanceRoll}|inline"')
			surge_roll=None


	if surge_roll is not None:
		if tides:
			ch.mod_cc(tidesCC, 1)
			field_out.append(f'-f "{tidesCC} Recharged|{ch.cc_str(tidesCC)}|inline"')

		surgeRoll = vroll(surge_roll)
		surge = surge_db[ceil(surgeRoll.total / 2) - 1]
		field_out	.append(f'-f "Wild Surge|{chanceRoll}\n{surgeRoll} : {surge.desc}"')
		if int(get('SorcererLevel',0))>=14 and not surge_roll.isdecimal():
			controlRoll = vroll(surge_roll)
			controlSurge = surge_db[ceil(controlRoll.total / 2) - 1]
			field_out.append(f'-f "Controlled Surge|{controlRoll} :{controlSurge.desc}"')
			# don't apply either to not have to roll anything back, could show manual command
		else:
			if surge.type == 'roll':
				field_out.append(f'-f "Roll|{vroll(surge.roll)}|inline"')
			elif surge.type == 'health':
				hp_roll=vroll(surge.roll)
				ch.modify_hp(hp_roll.total, overflow=False)
				field_out.append(f'-f "Health gained|{hp_roll}\n{ch.hp_str()}|inline"')
			elif surge.type == 'cc':
				ch.set_cc(cc,ch.get_cc_max(cc))
				field_out.append(f'-f "{cc}|{ch.cc_str(cc)}"')
			elif surge.type == 'slots':
				usedSlots = [x for x in range(1, 10) if ch.spellbook.get_slots(x) < character().spellbook.get_max_slots(x)]
				if usedSlots:
					usedSlots=usedSlots[0]
					ch.spellbook.set_slots(usedSlots,ch.spellbook.get_slots(usedSlots) + 1)
					field_out.append(f'-f "Spell Slots|{ch.spellbook.slots_str(usedSlots)}|inline"')
				else:
					field_out.append(f'-f "Spell Slots|Free {spell_name}|inline"')
					args.append('-i')
			elif surge.type=='effect':
				if Combat:=combat():
					Combat.me.add_effect(surge.effect[0], surge.effect[2], surge.effect[1])
			elif surge.type=='cast':
				if surge.cast[2]=='self':
					cast_target=f'-t "{name}"'
				else:
					cast_target=''
				field_out.append(f'''-f "To cast|`{ctx.prefix}cast \\"{surge.cast[0]}\\" -i noconc -l {surge.cast[1]} {cast_target}`"''')
			elif surge.type=='text':
				pass
			else:
				err(f" unknown surge type {surge.type} for surge {surgeRoll}.")

return f"""cast "{spell_name}" {' '.join([(f'"{i}"' if ' ' in i else i) for i in args[1:]])} {" ".join(field_out)} {footer}"""
</drac2>