<drac2>
roster_var='npc_local_roster'
key='group'

#parse arguments
args=&ARGS&
if len(args)<2:
	return f'echo You must provide at least one group and one selection.'
group=args[0]
group=None if group.lower() in ['-','reset'] else group
selection=args[1:]

# load modifiable characters, only from user's private roster
roster=load_json(get(roster_var, '{}'))

grouped=[]

# set the group of all characters whose current group or id matches the selection
for id,c in roster.items():
	if cat:= c.get(key):
		if any(cat.lower().startswith(s.lower()) for s in selection):
			c[key] = group
			grouped.append(id)
	if any(id.lower().startswith(s.lower()) for s in selection):
		c[key] =group
		grouped.append(id)
	# clear it all the way
	if c.get(key, 'dont remove ') is None:
		c.pop(key)

# store
set_uvar(roster_var, dump_json(roster))

# output
grouped=', '.join(g for g in grouped) if grouped else "None"
if group:
	return f'echo {grouped} added to {group}'
else:
	return f'echo {grouped} removed from group.'

</drac2>
