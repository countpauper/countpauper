<drac2>
if ctx.alias in ('c','check'):
	err(f"The `victory` snippet can only be used with `!attack`. You can use `!{ctx.alias} acro bladesong` instead. ")
if ctx.alias in ('s','save'):
	err(f"The `victory` snippet can only  be used with `!attack`. You can use `!{ctx.alias} con bladesong` instead. ")
n, c = 'Bladesong', combat()
me = c.me if c else None
if (me and not me.get_effect(n)) or (not me and get(n,'').lower()!='true'):
	return '-f Bladesong|"Not Active" '
if int(get('WizardLevel',0)) < 14:
	return '-f "Song of Victory"|"Wizard Level<14" '
bonus = max(intelligenceMod,1)
return f'-d {bonus} -f "Song of Victory"|"Damage +{bonus}" -phrase "Singing victoriously." '
</drac2>