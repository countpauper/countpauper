<drac2>
n, c = 'Bladesong', combat()
me = c.me if c else None
if (me and not me.get_effect(n)) or (not me and get(n,'').lower()!='true'):
	return '-f Bladesong|"Not Active" '
bonus = max(intelligenceMod,1)
if ctx.alias in ('c','check'):
	return 'adv -f Bladesong|Advantage '
elif ctx.alias in ('s','save'):
	return f'-b {bonus}[Bladesong] -f Bladesong|"Bonus {bonus}(int)" '
elif ctx.alias in ('a','attack'):
	if int(get('WizardLevel',0)) >= 14:
		return f'-d {bonus} -f "Song of Victory"|"Damage +{bonus}" -phrase "Singing victoriously." '
	else:
		return '-f "Song of Victory"|"Wizard Level<14" '
else:
	err("The `bladesong` snippet can only be used with `!check acro`, `!save con` or `!attack` for song of victory. ")
</drac2>