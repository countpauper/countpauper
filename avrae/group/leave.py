<drac2>
args="&*&"

if not args:
	args=name

c=combat()
if not c:
	return f'echo The channel needs to be in initiaive. Use `{ctx.prefix}{ctx.alias} begin` or `{ctx.prefix}i begin`'

# find the group name
group=None
for g in c.combatants:
	names=[args]+args.split()
	for n in names:
		if g.get_effect(n):
			group=g.name
			removed=n
			return f'i re "{group}" "{removed}"'

return f'echo No one named `{args}` is part of a {ctx.alias}.'
</drac2>
