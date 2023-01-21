# TODO:
#  - cast (or just cast without subcommand but argument)
#  - reset
#  [done] -l -dc and -b options for store
#  show dc and bonus for
!alias brss <drac2>
c=character()
spellStore=load_json(c.get_cvar('spellStore','{}'))
spells=spellStore.get("spells",[])
capacity=spellStore.get("capacity",5)
args=&ARGS&
title='Ring of Spell Storing'
desc='''This ring stores spells cast into it, holding them until the attuned wearer uses them. The ring can store up to 5 levels worth of spells at a time.'''
levelStr=["Unknown","1st","2nd","3rd","4th","5th","6th", "7th", "8th","9th","10th"]
spellStrs=[]
for id,s in enumerate(spells):
	props=[f'{levelStr[s.get("level",0)]} level']
	if dc:=s.get('dc'):
		props.append(f'DC {dc}')
	if atk:=s.get('attack'):
		props.append(f'Attack {"+" if atk>=0 else ""}{atk}')
	spellStrs.append(f'{id} - {s.get("name","unknown").title()} ({", ".join(props)})')
spellDesc='\n'.join(spellStrs) or 'None'
levelSum=sum(s.get("level",0) for s in spells)
return f'embed -title "{title}" -desc "{desc}" -f "Stored Spells ({levelSum}/{capacity})|{spellDesc}" -thumb {image} -color {color}'
</drac2>


!alias bstore <drac2>
def FindSpell(n, maxLvl=5):
	spellDb = load_yaml(get_gvar('7226e680-f21b-43f2-a08a-3065190109e8'))
	spellDb = [s for s in spellDb if s.level >= 1 and s.level <= maxLvl]
	spells={s.name:s.level for s in spellDb if s.name.lower()==spellName}
	if not spells:
		spells={s.name:s.level for s in spellDb if s.name.lower().startswith(spellName)}
	if not spells:
		spells={s.name:s.level for s in spellDb if spellName in s.name.lower()}
	if not spells:
		return n, None
	else:
		n=list(spells.keys())[0]
		return (n, spells[n])

c=character()
cvn='spellStore'
spellStore=load_json(c.get_cvar(cvn,'{}'))
spells=spellStore.get("spells",[])
capacity=spellStore.get("capacity",5)
levelSum=sum(s.get("level",0) for s in spells)
args=&ARGS&
if not args:
	return f'help {ctx.alias} store -here'
spellName=args.pop(0)
args=argparse(args)
# fuzzy match spell name
spell,spell_lvl = FindSpell(spellName, capacity)
lvl=args.last('l', spell_lvl, type_=int)
if lvl is None:
	return f'echo Unknown spell `{spellName.title()}`.'
if lvl<spell_lvl:
	return f'echo `{spellName.title()}` can not be cast at level `{lvl}`'
levelStr=["Unknown","1st","2nd","3rd","4th","5th","6th", "7th", "8th","9th","10th"]
if lvl+levelSum>capacity:
	return f'''echo There isn't enough room to store {spell.title()} ({levelStr[lvl]} level) into the Ring of Spell Storing.'''
spells.append(dict(name=spell, level=lvl, dc=args.last('dc', type_=int), attack=args.last('b', type_=int)))
spellStore['spells']=spells
c.set_cvar(cvn, dump_json(spellStore))
return f'embed -title "Storing {spell.title()}" -desc "{spell.title()} ({levelStr[lvl]} level) has been stored into the Ring of Spell Storing." '
</drac2>


!alias brssc <drac2>
def FindStoreIndex(n, store):
	ids=[id for id, s in enumerate(store) if s.name.lower()==n]
	if not ids:
		ids=[id for id, s in enumerate(store) if s.name.lower().startswith(n)]
	if not ids:
		ids=[id for id, s in enumerate(store) if n in s.name.lower()]
	if ids:
		return ids[0]
	else:
		return None

c=character()
cvn='spellStore'
spellStore=load_json(c.get_cvar(cvn,'{}'))
spells=spellStore.get("spells",[])
args=&ARGS&
if not args:
	return f'help {ctx.alias} store -here'
spellArg=args.pop(0).lower()
if spellArg.isdecimal():
	spellIdx=int(spellArg)
	if spellIdx>=len(spells):
		return f'echo There is no #`{spellIdx}` spell stored. Use `{ctx.prefix}{ctx.alias}` to see the stored spells.'
else:
	spellIdx=FindStoreIndex(spellArg, spells)
if spellIdx is None:
	return f'echo The spell `{spellArg}` is not stored. Use `{ctx.prefix}{ctx.alias}` to see the stored spells.'
s=spells.pop(spellIdx)

cast_args=[f'-title "{name} casts {s.name.title()} from {get("their","their")} Ring of Spell Storing."']
if dc:=s.get('dc'):
	cast_args.append(f'-dc {dc}')
if b:=s.get('attack'):
	cast_args.append(f'-b {b-c.spellbook.sab}')
if l:=s.get('level'):
	cast_args.append(f'-l {l}')
cast_args+=[f'"{a}"' for a in args]

if '-i' not in args:
	spellStore['spells']=spells
	c.set_cvar(cvn, dump_json(spellStore))
else:
	cast_args.append(f'-f "Ring of Spell Storing|{s.name.title()} remains stored."')

return f'cast "{s.name}" -i {" ".join(cast_args)}'
</drac2>