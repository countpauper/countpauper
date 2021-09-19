!alias rspell <drac2>
spell_db=spell_list=load_json(get_gvar('13dc3e0a-a230-40ca-8fb3-a39846300b18'))
args=argparse(&ARGS&)
levels=args.get('l',type_=int) or range(10)
spells=[n for n,p in spell_db.items() if p.level in levels]
if not spells:
	return f'echo No spells {levels}'
spell=spells[randint(len(spells))]

return f'spell "{spell}"'
</drac2>