!snippet cc <drac2>
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
with_stat=''
if cc.desc:
    for a in ["intelligence","wisdom","charisma"]:
        if a in cc.desc:
            with_stat=f'-with {a[:3]}'
if not cc.value:
    err(f'{cc.name} not available')
cc.mod(-1)
return f'-f "{cc.name}|{cc.full_str()}" -i bla {with_stat}'
</drac2>