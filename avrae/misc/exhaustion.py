!servsnippet zzz
<drac2>
c=character()
if not c:
    return ''
cc='Exhaustion'
if not c.cc_exists(cc):
    return ''

diskey='dis'
exhaustion={
    0: {},
    1: {diskey:['c','check'], 'desc':'Ability checks'},
    2: {},
    3: {diskey: ['a','attack','s','save'], 'desc':'Attack Rolls, Saving throws'},
    4: {},
    5: {},
    6: {}
}

disadvantage = False
all_desc=[]
for lvl in range(0,c.get_cc(cc)+1):
    if ctx.alias in exhaustion[lvl].get(diskey,[]):
        disadvantage=True
    if desc:=exhaustion[lvl].get('desc'):
        all_desc.append(desc)
if not all_desc:
    all_desc.append('Nothing')
return ('dis ' if disadvantage else '')+f'-f "{cc}|{cc_str(cc)}\nDisadvantage on {", ".join(all_desc)}"'
</drac2>