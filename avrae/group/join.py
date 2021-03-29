<drac2>
args=&ARGS&

if not args:
	return f'help {ctx.alias} join -here'

group=args[0]
joiner=ctx.author.display_name
if len(args)>1:
	joiner=" ".join(args[1:])

c=combat()
if not c:
	return f'echo The channel needs to be in initiaive. Use `{ctx.prefix}{ctx.alias} begin or `{ctx.prefix}i begin`'

# clean the character from other/existing groups
for g in c.combatants:
	g.remove_effect(joiner.lower())

return f'i effect "{group}" "{joiner}"'
</drac2>
