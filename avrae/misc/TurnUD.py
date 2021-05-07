<drac2>
args=argparse('&*&')

# get all the targets in combat from the -t argument
targets=args.get('t')
c=combat()
if targets and not c:
	return f"echo Your channel needs to be in initiative to turn undead."
elif c:
	targets={n:c.get_combatant(n) for n in targets}
	unknown=[n for n,t in targets.items() if not t]
	if unknown:
		return f'echo Some undead were not found: {",".join(unknown)}.'
else:
	targets=dict()

# Use the consumable
ch=character()
cc='Channel Divinity'
if not args.get('i'):
	if not ch.cc_exists(cc):
		return f"echo {name} doesn't have *{cc}*."
	if ch.get_cc(cc)<1:
		return f"echo {name} is out of *{cc}* : {ch.cc_str(cc)}."
	ch.mod_cc(cc,-1)
	cc_field=f'-f "{cc}|{ch.cc_str(cc)}"'
else:
	cc_field=f'-f "{cc}|*Ignored*"'

# have all targets roll saves
dc=ch.spellbook.dc
saves={t:vroll(t.saves.get('wis').d20(args.adv(boolwise=True,ephem=True))) for t in targets.values()}

# apply the effect
[t.add_effect('Turned','',duration=10) for t,s in saves.items() if s.total<dc]
result=[f'**{t.name}** : WIS Save: {s} vs DC {dc} ; {"**Turned**" if s.total<dc else "*Saved*"}' for t,s in saves.items()]

# format the embed
nl='\n'
desc="""-f "Effect|As an action, you present your holy symbol and speak a prayer censuring the undead. Each undead that can see or hear you within 30 feet of you must make a Wisdom saving throw. If the creature fails its saving throw, it is turned for 1 minute or until it takes any damage.

A turned creature must spend its turns trying to move as far away from you as it can, and it can't willingly move to a space within 30 feet of you. It also can't take reactions. For its action, it can use only the Dash action or try to escape from an effect that prevents it from moving. If there's nowhere to move, the creature can use the Dodge action." """
return f'embed -title "{name} turns the undead!" {cc_field} {desc} -desc "{nl.join(result)}" -thumb {image} -color {color}'
</drac2>
