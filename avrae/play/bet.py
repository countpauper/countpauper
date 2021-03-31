<drac2>
cvar='Play'
game_name='bet'
state=load_json(get(cvar,'{}'))
bet=state.get(game_name)
if bet is not None:
	bet=int(bet)
args="&*&".lower()
base_cmd=f'{ctx.prefix}{ctx.alias}'
syntax=f'{base_cmd} {game_name} [<amount>]'
if not args:
	if bet:
		amount=bet
		title=f'{name} previously placed a bet.'
	else:
		return f'echo You haven\'t placed a bet yet. Use `{base_cmd} {game_name} <amount>` to do so.'
elif bet:
	return f'echo You still have placed a bet of `{bet}`. First drop out with `{base_cmd} drop` or collect with `!{base_cmd} collect`'
elif args.isdigit():
	title=f'{name} places a bet.'
	amount=int(args)
else:
	return f'echo {args} is not a valid amount. Use `{syntax}`'

# persist
state[game_name]=str(amount)
character().set_cvar(cvar,dump_json(state))

# output
desc=f':coin: {amount}'
return f'embed -title "{title}" -desc "{desc}" -footer "{syntax} | {base_cmd} raise <amount> | {base_cmd} drop."'
</drac2>