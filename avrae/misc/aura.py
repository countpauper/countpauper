!alias aura <drac2>
aura='Aura of Protection'
C=combat()
argstr='''&*&'''
if not C:
	return f'a "{aura}" {argstr}'
args=argparse(argstr)
me=C.me
if me and me.hp==0 or me.get_effect('Unconscious'):
	targets={}
else:
	phrase=''
	targets={t:C.get_combatant(t) for t in args.get('t')}
	targets[me.name]=me
	n=me.note
	overlay_notes = ['Overlay8: uc12~a0e080{targ}', f'Effect8: Aura of Protection / {me.name}']
	if "Overlay8" not in n:
		overlay_notes=([n] if n else []) + overlay_notes
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