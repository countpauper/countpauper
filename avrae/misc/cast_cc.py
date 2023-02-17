<drac2>
args=&ARGS&
if not args:
	return ''
spell=args[0].lower()
c = character()
ccs=[cc for cc in c.consumables if spell==cc.name.lower()]
ccs+=[cc for cc in c.consumables if cc.name.lower().startswith(spell)]
ccs+=[cc for cc in c.consumables if spell in cc.name.lower()]
if not ccs:
	return f'-f "{spell}|No counter"'
cc=ccs[0]
if not cc.value:
	return f'-f "{cc.name}|Not available"'
cc.mod(-1)
return f'-f "{cc.name}|{cc.full_str()}" -i bla'
</drac2>