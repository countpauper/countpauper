!alias hide <drac2>
C=combat()
if not C:
	return f"Can only {ctx.prefix}{ctx.alias} in combat"
targets=[t for t in &ARGS&]
if "dead" in targets:
	targets.remove('dead')
	for c in C.combatants:
		if c.creature_type and c.hp<=0 and c.name not in targets:
			targets.append(c.name)
targets={t:C.get_combatant(t) for t in targets}
loc_prefix='Location: '
hidden=dict()
for n,t in targets.items():
	if not t:
		hidden[n]="who?"
	elif not t.note:
		hidden[n]="where?"
	else:
		notes=[n.strip() for n in t.note.split("|")]
		cleaned_notes=[n for n in notes if not n.startswith(loc_prefix)]
		t.set_note(" | ".join(n for n in cleaned_notes))
		hidden[n]="yes" if t.hp>0 else 'dead'
return f'echo Hidden: {", ".join(f"{n}: {desc}" for n,desc in hidden.items())}'
</drac2>