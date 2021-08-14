<drac2>
# load the appropriate !play state for the player
uvar='Play_er'
player_state=load_json(get(uvar,'{}'))
if player:=player_state.get('player'):
	state=player_state.setdefault(player,{})
	they='they'
	their='their'
	image='none'
	color = 0
else:
	cvar = 'Play'
	player_state=None
	state=load_json(get(cvar, '{}'))
	player = name
	they=get("they","they")
	their=get("their","their")

game_name='bet'
bet=state.get(game_name)

args="&*&".lower()
base_cmd=f'{ctx.prefix}{ctx.alias}'
syntax=f'{base_cmd} drop|out'
if not bet:
	return f'echo You haven\'t placed a bet yet. Use `{base_cmd} {game_name} <amount> [<coin>]` to do so.'

# persist
state.pop(game_name)
if player_state is not None:
	set_uvar(uvar, dump_json(player_state))
else:
	character().set_cvar(cvar,dump_json(state))

# output
desc=f'{they.capitalize()} lost {their} bet of {", ".join(f"{q} {c}" for c,q in bet.items())}'

return f'embed -title "{player} drops out." -desc "{desc}" -color {color} -footer "{syntax}" -thumb {image}'
</drac2>