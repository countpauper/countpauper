<drac2>
args="&*&"

if not args:
	args=ctx.author.display_name

c=combat()
if not c:
	return f'echo The channel needs to be in initiaive. Use `{ctx.prefix}{ctx.alias} begin or `{ctx.prefix}i begin`'

# find the group name
group=None
for g in c.combatants:
	if g.get_effect(args):
		group=g.name
		break

if not group:
	return f'echo No one named `{args}` is part of a {ctx.alias}.'

return f'i re "{group}" "{args}"'
</drac2>
