!alias smuggle echo <drac2>
args=&ARGS&
if not args:
	return f'`!{ctx.alias} <attack> [-t <target>]...`'

if c:=combat():
	targets = argparse(args).get('t')
	targets=[c.get_combatant(t) for t in targets]
if not targets:
	targets = [character()]
attacks=[]
name = args[0]
for t in targets:
	if t:
		attacks+=[a.raw for a in t.attacks if name.lower() in a.name.lower()]
return f'`{attacks}`'
</drac2>
