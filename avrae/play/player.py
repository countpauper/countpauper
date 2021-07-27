!alias bp <drac2>
# TODO: this should go in an uvar, which one? and then jank it up knowing which data to store the state in killer

uvar='Play_ers'
name_name='player'
state=load_json(get(uvar,'{}'))

if args:="&*&":
	if args.lower() in ['reset','none',name.lower()]:
		if name_name in state:
			state.pop(name_name)
	else:
		state[name_name]=args

player=state.get(name_name,name)
set_uvar(uvar,dump_json(state))

return f'echo The current player is `{player}`.'
</drac2>
