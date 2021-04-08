<drac2>
cvar='Play'
game_name='bet'
state=load_json(get(cvar,'{}'))
bet=state.get(game_name,{})
args="&*&".lower()
base_cmd=f'{ctx.prefix}{ctx.alias}'
syntax=f'{base_cmd} drop|out'
if not bet:
	return f'echo You haven\'t placed a bet yet. Use `{base_cmd} {game_name} <amount> [<coin>]` to do so.'

# persist
state.pop(game_name)
character().set_cvar(cvar,dump_json(state))

# output
desc=f'{get("they","they").capitalize()} lost {get("their","their")} bet of {", ".join(f"{q} {c}" for c,q in bet.items())}'

return f'embed -title "{name} drops out." -desc "{desc}" -color {color} -footer "{syntax}"'
</drac2>