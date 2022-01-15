!alias twi <drac2>
c=combat()
if not c:
	return f'echo `{ctx.prefix}{ctx.alias} only works in combat.'
if arg:='''&*&''':
	t=c.get_combatant(arg)
	if not t:
		return f'echo Unrecognzied combatant `{arg}`'
else:
	t = c.me or c.current
	if not t:
		return f'echo No active combatant. Use `{ctx.prefix}{ctx.alias} <target>`'
clerics={cleric:lvl for cleric in c.combatants if cleric.get_effect('Twilight Sanctuary') and (lvl:=cleric.levels.get('Cleric'))>=2}
if not clerics:
	return f'echo No clerics with Twilight Sanctuary found.'
cleric=list(clerics.keys())[0]	# TODO: nearest and within range or highest level in range
cleric_level = clerics[cleric]

if e := t.get_effect('charmed') or t.get_effect('frightened'):
	t.remove_effect(e.name)
	return f'''embed -title "{cleric.name}'s Twilight sanctuary" -desc "Removed {e.name} from {t.name}"'''
else:
	thp = t.temp_hp
	if thp<cleric_level:
		t.set_temp_hp(cleric_level)
		return f'''embed -title "{cleric.name}'s twilight sanctuary" -desc "Gave {t.name} temporary hitpoints {t.hp_str()}"'''
	else:
		return f'''embed -title "{cleric.name}'s twilight sanctuary" -desc "Kept {t.name}'s temporary hitpoints {t.hp_str()}"'''
</drac2>