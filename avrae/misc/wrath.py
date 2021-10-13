!alias wrath <drac2>
C=combat()
args='''&*&'''
if not args:
	return f'echo Provude a target.'
if not C:
	return f'echo Not in combat.'
target=argparse(args).last('t') or '''&1&'''
c=C.get_combatant(target)
if not c:
	return f'echo Target {target} was not found.'
dex_save=c.saves.get('dex')
str_save=c.saves.get('str')

save_advantages=[s.lower()[:3] for e in c.effects for s in e.effect.get('sadv',[]) ]
save_disadvantages=[s.lower()[:3] for e in c.effects for s in e.effect.get('sdis',[]) ]
reason=""
save=None

if dex_save.adv or 'dex' in save_advantages:
	reason='it has advantage'
	save='Dexterity'
elif str_save.adv or 'str' in save_advantages:
	reason='it has advantage'
	save='Strength'
elif dex_save.adv is False or 'dex' in save_disadvantages:
	reason='Dexterity saves have disadvantage'
	save='Strength'
elif str_save.adv is False or 'str' in save_disadvantages:
	reason = 'Strength saves have disadvantage'
	save='Dexterity'
elif dex_save.value>str_save.value:
	reason='it has the highest bonus'
	save = 'Dexterity'
elif dex_save.value<str_save.value:
	reason='it has the highest bonus'
	save = 'Strength'
else:
	reason = 'it doesn\'t matter'
	save = 'Strength'

return f'a "Channel Divinity: Nature\'s Wrath ({save})" {args} -f "{save} save was chosen, because {reason}"'
</drac2>
