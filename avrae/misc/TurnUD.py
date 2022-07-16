<drac2>
argstr="""&*&"""
args=argparse(argstr)

# get all the targets in combat from the -t argument
target_args=[t.split('|') for t in args.get('t')]
targets=[ta[0] for ta in target_args]
[args.add_context(ta[0], argparse(" ".join(ta[1:]))) for ta in target_args]

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
saves={}
for n,t in targets.items():
	args.set_context(n)
	if args.last('fail'):
		saves[t]=False
	elif args.last('pass'):
		saves[t]=True
	else:
		saves[t]=vroll('+'.join([t.saves.get('wis').d20(args.adv(boolwise=True,ephem=True))]+args.get('b',ephem=True)))

# destroy undead
if (destroy:=[None,None,None,None,None,0.5,0.5,0.5,1,1,1,2,2,2,3,3,3,4,4,4,4][ch.levels.get('Cleric')]):
	destroyed = [t for t,s in saves.items() if s is False and (s is not True or s.total<dc) and t.levels.total_level<=destroy]
	[t.set_hp(0) for t in destroyed]
else:
	destroyed=[]

# apply the turned effect
turned = [t for t,s in saves.items() if s and s is False and (s is not True or s.total<dc) and not t in destroyed]
[t.add_effect('Turned',duration=10) for t in turned]

result=[f'**{t.name}** : WIS Save: {"fail" if s is False else "pass" if s is True else s} vs DC {dc} ; {"**Turned**" if t in turned else "**Destroyed**" if t in destroyed else "*Saved*"}' for t,s in saves.items()]

# format the embed
nl='\n'
desc="""-f "Turn Undead|As an action, you present your holy symbol and speak a prayer censuring the undead. Each undead that can see or hear you within 30 feet of you must make a Wisdom saving throw. If the creature fails its saving throw, it is turned for 1 minute or until it takes any damage.

A turned creature must spend its turns trying to move as far away from you as it can, and it can't willingly move to a space within 30 feet of you. It also can't take reactions. For its action, it can use only the Dash action or try to escape from an effect that prevents it from moving. If there's nowhere to move, the creature can use the Dodge action." """
if destroy is not None:
	if destroy < 1:
		destroy = f'1/{int(1 / destroy)}'
	desc+=f""" -f "Destroy Undead|When an undead fails its saving throw against your Turn Undead feature, the creature is instantly destroyed if its challenge rating is at or below [{destroy}]." """

return f'embed -title "{name} turns the undead!" {cc_field} {desc} -desc "{nl.join(result)}" -thumb {image} -color {color}'
</drac2>
