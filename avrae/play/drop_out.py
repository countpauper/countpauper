<drac2>
cvar='Play'
game_name='bet'
state=load_json(get(cvar,'{}'))
bet=state.get(game_name)
if bet is not None:
	bet=int(bet)
args="&*&".lower()
base_cmd=f'{ctx.prefix}{ctx.alias}'
syntax=f'{base_cmd} drop|out'
if not bet:
	return f'echo You haven\'t placed a bet yet. Use `{base_cmd} {game_name} <amount>` to do so.'

# persist
state.pop(game_name)
character().set_cvar(cvar,dump_json(state))

# output
desc=f'{get("they","they").capitalize()} lost {get("their","their")} bet of {bet} :coin:'
if exists('bags'):
	desc+=f'\nPay up using `!coins -{bet}`'

return f'embed -title "{name} drops out." -desc "{desc}" -footer "{syntax}"'
</drac2>