<drac2>
args=&ARGS&
pargs = argparse(args)
if not args:
	return f'echo {ctx.prefix}{ctx.alias} "<spell>" [<cast arguments>] [-i] [-with <stat>]'

# find custom counter cc matching the spell name
spell=args[0].lower()
ch=character()
ccs=[cc for cc in ch.consumables if spell==cc.name.lower()]
if not ccs:
	ccs = [cc for cc in ch.consumables if spell in cc.name.lower()]
if not ccs:
	return f'echo You do not know {spell}.'
if len(ccs)>1:
	return f'echo Select either {", ".join(ccs)}'
cc=ccs[0]
spell=cc.name
args=args[1:]

# figure out -with: 1) no attack or save doesn't need it. 2) explicit argument 3) single stat mention description of cc 4) automatic highest mentioned stat in description of cc
spell_db=load_json(get_gvar('13dc3e0a-a230-40ca-8fb3-a39846300b18'))
db_spell=spell_db.get(spell.lower(),{'save':'unknown','attack':True})
cast_with = ''
if not pargs.last('with') and ('save' in db_spell or 'attack' in db_spell):
	desc=cc.desc if cc.desc else ''
	cs = [s for s in ['intelligence','wisdom','charisma'] if s in desc.lower()]
	if not cs:
		cs = [s for s in ['int','wis','cha','con'] if s in desc.lower().split()]
	if not cs:
		return f'echo Unknown spell casting modifier. Use `-with int|cha|wis` or add the spell casting modifier in the *description* of `{cc.name}` using `!cc create "{cc.name}" -min {cc.min} -max {cc.max} {f"-type bubble" if cc.display_type else ""} -desc "Cast with intelligence|wisdom|charisma. {desc}"`'
	if len(cs)==1:
		cs=cs[0]
	else:	 # auto pick highest
		cs={s:ch.stats.get_mod(s) for s in cs}
		cs=[s for s,score in cs.items() if score==max(cs.values())][0]
	cast_with=f'-with {cs[:3]}'

# use custom counter

if '-i' in args:
	field=f'-f "{cc.name} : Unused|{cc.desc}"'
	args.remove('-i')
elif cc.value<1:
	return f'echo {cc.name} : {cc}'
else:
	cc.set(cc.value-1)
	field=f'-f "{cc.name} {cc}|{cc.desc}"'

return f"""cast "{spell}" {" ".join(f'"{a}"'for a in args)}) {field} {cast_with} -i"""
</drac2>