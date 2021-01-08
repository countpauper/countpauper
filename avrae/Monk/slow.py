<drac2>
args="&*&"
ch=character()
MonkLevel = int(get('MonkLevel', 0))
dmg = None
if args:
	if not args.isdigit():
		return f'echo Use: `{ctx.prefix}{ctx.alias} slow [<damage applied by fall>]`\nIf the damage is specified, the user\'s hitpoints are automatically corrected.'
	else:
		dmg=int(args)
mitigated=MonkLevel * 5
fields=f'-desc "you can use your Reaction when you fall to reduce any Falling damage you take by an amount equal to five times your monk level [{mitigated}]."'
if dmg is not None:
	remaining = max(0,dmg-mitigated)
	mitigated = dmg-remaining
	fields+=f' -f "Fall Damage|~~{dmg}~~ {remaining}|inline"'

	ch.modify_hp(mitigated, overflow=False)
	fields+=f' -f "{name}|{ch.hp_str()}|inline"'

return f'embed -title "{name} falls slow!" {fields}'
</drac2>