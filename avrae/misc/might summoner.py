<drac2>
c=combat()
if not c or not c.me:
	return f'echo Join initiative before making your summons mighty.'

def IsControlledBeast(beast, controller):
	if beast.controller!=controller:
		return False
	#if not any(cls=='Monster' for cls, lvl in beast.levels):
	#	return False
	return beast.creature_type in ['beast','fey']

args=argparse(&ARGS&)
hd=args.last('hd',1,type_=int)
controlled_beasts=[beast for beast in c.combatants if IsControlledBeast(beast, c.me.controller)]
# TODO: parent summoner effect on user?
creatures = load_json(get_gvar('78cb6fe5-90c7-4f7a-b359-c8441a2f7b22'))

def ParseHitDice(s):
	if s is None:
		return None
	if (d_idx:=s.find('d')) >= 0:
		return int(s[:d_idx])
	elif s.isdecimal():
		return s
	else:
		return None
result=dict()
for beast in controlled_beasts:
	creature_hd = args.last('hd',ParseHitDice(creatures.get(beast.monster_name.lower(),dict()).get('hd')) or 0,type_=int)
	result[beast.name]=f'{beast.monster_name} [{creature_hd}HD]'
	extra_hp=2*creature_hd
	beast.add_effect('Mighty', passive_effects=dict(magical_damage=True,max_hp_bonus=extra_hp))
	beast.modify_hp(extra_hp,overflow=False)
	beasts={n:p for n,p in creatures.items() if p.get('')}
return f'''echo {", ".join(f'{b}: {d}' for b, d in result.items())} are mighty.'''
</drac2>
