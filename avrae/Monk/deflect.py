<drac2>
args="&*&"
ch=character()
MonkLevel = int(get('MonkLevel', 0))
dmg = None
if args:
	if not args.isdigit():
		return f'echo Use: `{ctx.prefix}{ctx.alias} deflect [<damage applied by ranged attack>]`\nIf the damage is specified, the user\'s hitpoints are automatically corrected.'
	else:
		dmg=int(args)
r=vroll(f'1d10+{dexterityMod}+{MonkLevel}')
fields=f'-desc "You can use your **reaction** to deflect or catch the missile when you are hit by a *ranged weapon attack*. When you do so, the damage you take from the attack is reduced by 1d10 + your Dexterity modifier + your monk level [{r}]."'
if dmg is not None:
	mitigated=min(dmg,r.total)
	remaining = dmg-mitigated
	fields+=f' -f "Damage|~~{dmg}~~ {remaining}|inline"'

	ch.modify_hp(mitigated, overflow=False)
	fields+=f' -f "{name}|{ch.hp_str()}|inline"'

	if remaining<=0:
		fields+=f' -f "Missile Caught|`{ctx.prefix}{ctx.alias} return <damage dice> [<attack options>]`|inline"'
return f'embed -title "{name} deflects a missile!" {fields}'
</drac2>