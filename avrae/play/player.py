<drac2>
uvar='Play_er'
name_name='player'
state=load_json(get(uvar,'{}'))

if args:="&*&":
	if 	args.lower() in ['reset','none',name.lower(), name.lower().split()[0]]:
		if name_name in state:
			state.pop(name_name)
	else:
		state[name_name]=args

player=state.get(name_name,name)
set_uvar(uvar,dump_json(state))

return f'echo The current player is `{player}`.'
</drac2>
