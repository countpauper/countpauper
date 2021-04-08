<drac2>
cvar='Play'
game_name='bet'
state=load_json(get(cvar,'{}'))
bet=state.get(game_name,{})
base_cmd=f'{ctx.prefix}{ctx.alias}'
syntax=f'{base_cmd} collect [<winnings> [<coin>]]'

# get !coins pouch if any
bags = load_json(get('bags', '{}'))
coins = ([b[1] for b in bags if b[0].lower().startswith('coin')] + [None])[0]

# parse arguments for profit
args="&*&".lower()

if args:
	if coins:
		for coin in coins.keys():	# convert 10cp => 10 cp for split
			args=args.replace(coin,f' {coin}')
		args=args.split()
		if len(args)==1:	# default select last bet coin or gp if none
			args.append((['gp']+list(bet.keys()))[-1])
		# parse the arguments to a dictionary
		coin_idx={c:args.index(c)-1 for c in coins.keys() if c in args}
		winnings={c:int(args[i]) for c,i in coin_idx.items() if i>=0 and args[i].isdigit()}
	else:
		args="&*&"
		chips=(['chips']+list(bet.keys()))[-1]
		if not args.isdigit():
			return f'echo Since you have no `!coins` pouch you can only collect {chips} `{base_cmd} {game_name} [<amount>]'
		winnings={chips:int(args)}
else:
	winnings={}
desc=f':coin: {", ".join(f"{q} {c}" for c,q in bet.items()) if bet else "Nothing"}'
desc+=f'\n:heavy_plus_sign: {", ".join(f"{q} {c}" for c,q in winnings.items())}' if winnings else ''

# update and persist !coins
if coins:
	for c,q in coins.items():
		coins[c]=q + winnings.get(c,0) + bet.get(c,0)
	desc+=f'\n:moneybag: {", ".join(f"{q} {c}" for c,q in coins.items() if q!=0)}'
	character().set_cvar('bags',dump_json(bags))

# persist bet
if game_name in state:
	state.pop(game_name)
character().set_cvar(cvar,dump_json(state))

return f'embed -title "{name} wins the bet." -desc "{desc}" -color {color} -footer "{syntax}"'
</drac2>