<drac2>
UVAR_LOCAL='npc_local_roster'
GROUP_KEY='group'

# load modifiable characters, only from user's private roster
roster=load_json(get(UVAR_LOCAL, '{}'))

#parse arguments
args=&ARGS&
if not args:
	groups={c.get(GROUP_KEY) for c in roster.values()}
	if None in groups:
		groups.remove(None)

	if not groups:
		return f'echo No local npcs are in a group. Use `{ctx.prefix}{ctx.alias} group -` to see them.'
	else:
		return f'echo Local groups: {", ".join(groups)}. Use `{ctx.prefix}{ctx.alias} group <group name>` to see who\'s in them.'

group=args[0]
group=None if group.lower() in ['-','reset'] else group
selection=args[1:]


# no selection: show current content
if not selection:
	grouped=[]
	if group is None:
		grouped=[id for id,c in roster.items() if c.get(GROUP_KEY) is None]
	else:
		grouped=[id for id,c in roster.items() if c.get(GROUP_KEY,'').lower()==group.lower()]
	group_list=', '.join(g for g in grouped) if grouped else "None"
	return f'echo {group_list} {"is" if len(grouped)==1 else "are"} in {group} and local.'

# set the group of all characters whose current group or id matches the selection
grouped=[]
for id,c in roster.items():
	if current:= c.get(GROUP_KEY):
		if any(current.lower().startswith(s.lower()) for s in selection):
			c[GROUP_KEY] = group
			grouped.append(id)
	if any(id.lower().startswith(s.lower()) for s in selection):
		c[GROUP_KEY] =group
		grouped.append(id)
	# clear it all the way
	if c.get(GROUP_KEY, 'dont remove ') is None:
		c.pop(GROUP_KEY)

# store
set_uvar(UVAR_LOCAL, dump_json(roster))

# output
grouped=', '.join(g for g in grouped) if grouped else None
warning = 'Only NPCs from your private roster can be modified.'
if group:
	return f'echo {grouped} added to {group}.{"" if grouped else f" {warning}"} '
else:
	return f'echo {grouped} removed from group.{"" if grouped else f" {warning}"}'
</drac2>
