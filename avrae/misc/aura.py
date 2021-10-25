!alias maura <drac2>
# TODO: -over 5 or -over5 select other default underlay #
# TODO: parent aura to caster to only remove those of this paladin
# TODO: check character().actions for aura for nicer error
# auto mode that parses distances
aura='Aura of Protection'
C=combat()
argstr='''&*&'''
if not C:
	return f'a "{aura}" {argstr}'

args=argparse(argstr)
effect_name=aura
if me:=C.me:
	effect_name=aura
	parent_effect = me.effects.get_effect(effect_name)
	if me and me.hp==0 or me.get_effect('Unconscious'):
		targets={}	 # TODO: drop parent aura
		me.remove_effect(effect_name)
	else:
		parent_effect
		# TODO: add or get parent aura
		phrase=''
		targets={t:C.get_combatant(t) for t in args.get('t')}
		if note:=me.note:
			overlay_notes=[n.strip() for n in note.split('|') if 'overlay8' not in n.lower()]
		else:
			overlay_notes=[]
		overlay_color = args.last('c', 'a0e080').lower()
		if len(overlay_color) == 6:
			overlay_color = '~' + overlay_color
		overlay_notes += [f'Overlay8: uc12{overlay_color}'+'{targ}', f'Effect8: Aura of Protection / {me.name}']
		me.set_note(' | '.join(overlay_notes))

unknown=[t for t,c in targets.items() if not c]
targets={t:c for t,c in targets.items() if not t in unknown}
removed=[]
remaining=[]
applied=[]
for affected in C.combatants:
	if any(t.name==affected.name for t in targets.values()):
		applied.append(affected)
	elif affected.get_effect(aura):
		removed.append(affected)
		affected.remove_effect(aura)

target_args=' '.join(f'-t "{t.name}"' for t in applied+remaining)
field_args=[]
sep=', '
if removed:
	field_args.append(f'-f "No longer protected|{sep.join(c.name for c in removed)}"')
if unknown:
	field_args.append(f'-f "Unrecognized|{sep.join(unknown)}"')
if target_args:
	return f'a "{aura}" {target_args} {" ".join(field_args)}'
else:
	return f'embed -title "{name} has no aura of protection" {" ".join(field_args)}'
</drac2>