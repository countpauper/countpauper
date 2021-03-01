<drac2>
arg="&*&"
roster_var='userRoster'
roster = load_json(get(roster_var, '[]'))
if not arg:
	return f'echo Use `{ctx.prefix}{ctx.alias} delete <id>|"<name>"`'

# select matches priority: case sensitive, exaxt match, id then name, partial match id or name
match=[u.d for u in roster if u.d==arg]
match+=[u.d for u in roster if u.d.lower()==arg.lower()]
match+=[u.d for u in roster if u.n.lower()==arg.lower()]
match+=[u.d for u in roster if u.d.lower().startswith(arg.lower())]
match+=[u.d for u in roster if u.n.lower().startswith(arg.lower())]

if not match:
	ids=[u.d for u in roster]
	return f'echo Select who to delete out of `{", ".join(ids)}`'
match=match[0]

# apply
newRoster=[u for u in roster if u.d!=match]
set_uvar(roster_var, dump_json(newRoster))

#report
return f'techo 2 {ctx.alias} character `{match}` deleted.'
</drac2>