<drac2>
## TODO -arcana: requires arcana check to cast or fizzles and uses a charge anyway
## TODO -desc description for the cc and or the embed?
## TODO dawn argument to reset at dawn instead of long rest and work together with !dawn
## Option to make bubbles cc

syntax = f'{ctx.prefix}{ctx.alias} ["<name>"] [<spells>[:<cost>]][...] [-charge <charge/day=1d6+1>] [-max <charges=10>] [-dc <dc=13>] [-break <dice=1d20>|never]'

spell_db=load_json(get_gvar('13dc3e0a-a230-40ca-8fb3-a39846300b18'))
wand_name=None
unknown=[]
spell_list={}
args=&ARGS&
for a in args:
	if a[0]=='-':
		break
	spell_name,cost = (a.lower().split(':',maxsplit=1)+[None,None])[:2]
	# 1: exact match
	spells=[s for s in spell_db.keys() if spell_name == s]
	# 2 partial match
	if not spells:
		spells=[s for s in spell_db.keys() if spell_name in s]
	if not spells:
		if not wand_name:
			wand_name = spell_name
		else:
			unknown.append(spell_name)
	else:
		spell_name = spells[0]
		spell_list[spell_name] = int(cost or max(spell_db[spell_name].get('level',0),1))
		if not wand_name:
			wand_name = f'wand of {spell_name}'
if not wand_name:
	return f"echo No name for the wand was provided. If no wand name is given the first spell is used as the wand's name. Use `{syntax}`"

aliasname=wand_name.lower().replace(' ','')
wand_name=wand_name.title().replace("'S","'s")
cc_name=wand_name


if unknown:
	return f'echo Unknown spells: {", ".join(unknown)}'
elif not spell_list:
	return f'echo No spells provided, Use `{syntax}`'

args=argparse(args)
code_vars=dict(reset=args.last('charge','1d6+1'),charges=args.last('max',10),spells=spell_list,wand=wand_name,cc=cc_name,dc=args.last('dc',13))
break_dice=args.last('break','1d20')
if break_dice.lower() in ['no','none','never','false',False,'0',0]:
	break_dice=None
else:
	code_vars['break']=break_dice

code="""<drac2>
### Automatically generated by !wand 
spells=%spells%
wand='%wand%'
cc='%cc%'"""

if len(spell_list)>1:	 # multi spell argument
	code+="""
# parse first argument
syntax=f'`{ctx.prefix}{ctx.alias} "<spell>" [-i] [-l <lvl>] [<cast arguments>]`'
args=&AR"""+"""GS&
if not args or args[0].lower() in ['?','help']:
	return f'echo Use: {syntax}.\\nSupported spells: {", ".join(spells.keys())}.'
spell=args[0]

# match the spell name
match=[sn for sn in spells.keys() if spell.lower()==sn.lower()]
if not match:
	match=[sn for sn in spells.keys() if spell.lower() in sn.lower()]
if not match:
	return f'echo Spell `{spell}` is not available with {wand}. Use: {syntax}.\\nSupported spells: {", ".join(spells.keys())}.\\nCase insensitive and partial name is supported.'
if len(match)>1:
	return f'echo Multiple spells match `{spell}`: {", ".join(match)}.'
match=match[0]
cost=spells[match]
"""
else:	# single spell, simplified syntax
	code+="""
# help argument
syntax=f'`{ctx.prefix}{ctx.alias} [-i] [-l <lvl>] [<cast arguments>]`'
args='&*"""+"""&'
if args.lower() in ['?','help']:
	return f'echo Use: {syntax}.\\nSupported spells: {", ".join(spells.keys())}.'
# select default spell
match,cost=list(spells.items())[0]
"""

code+="""
# create the CC
C=character()
C.create_cc_nx(cc, 0, %charges%, reset='long',reset_by='%reset%')

# parse spell arguments
cast_args='&*"""+"""&'
args=argparse(cast_args)
if args.last('i') or cost==0:
	fields=f'-f  "{cc}|No charges used|inline" '
else:
	# cost is level when using -l
	if lvl:=args.last('l'):
		cost=int(lvl)
	# remove the charges
	current=C.get_cc(cc)
	if cost>current:
		return f'echo Not enough charges [{cost}] on your {wand} to cast {match.title()} : {C.cc_str(cc)}'
	remaining = C.mod_cc(cc,-cost)
	fields=f' -f "{cc}[-{cost}]|{C.cc_str(cc)}|inline" '"""

if break_dice is not None:
	code+="""# Did it break?
	if remaining==0:
		r=vroll('%break%')
		broken='breaks!' if r.total==1 else 'retains its magic.'
		fields+=f'-f "Last charge expended|{r}\\nThe {wand} {broken}|inline" '
	"""
code+="""
dc=args.last('dc',int("%dc%"))
mod=args.last('mod',dc-8)

# perform the casting
return f'cast "{match}" -title "<name> casts [sname] with {get("their","their")} {wand}." -i {fields} -dc {dc} -mod {mod} {cast_args}'
</"""+"drac2>"


for var,val in code_vars.items():
	code=code.replace(f'%{var}%',str(val))

#return f'echo ```py\n!alias {aliasname} {code}```'
return f'alias {aliasname} {code}'
</drac2>