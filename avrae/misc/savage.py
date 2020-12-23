!alias savage <drac2>
arg=&ARGS&
if len(arg)!=3 or not arg[0].isdigit():
	return f'echo `!{ctx.alias} <original damage> <dice> <target>'
c=combat()
t=c.get_combatant(arg[2]) if c else None
if not t:
	return f'echo target "{arg[2]}" not found.'
r = vroll(arg[1])
org_dmg = int(arg[0])
dmg = max(0,r.total-org_dmg)
if dmg>0:
	t.mod_hp(-dmg)
	fields = f'-f "Savagery!|{r.dice}-{org_dmg}[original]=`{dmg}`|inline"'
	fields += f' -f "{t.name}|{t.hp_str()}|inline"'
else:
	fields = f'-f "Savagery insufficient|{r} <= {org_dmg}|inline"'

return f"""embed 
-title "{name} savagely attacks!"
-desc "Once per turn when you roll damage for a melee weapon attack, you can reroll the weapon's damage dice and use either total."
-footer "Concept by @""Nunberry#2818, code by @""Velglarn#4688"
{fields}"""
</drac2>