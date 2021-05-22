<drac2>
cvar='Play'
game_name='bet'
state=load_json(get(cvar,'{}'))
bet=state.get(game_name,{})
base_cmd=f'{ctx.prefix}{ctx.alias}'
syntax=f'{base_cmd} {game_name} [<amount> [<coin>]]'

desc = ''
args = "&*&".lower()

# get !coins pouch if any
bags = load_json(get('bags', '{}'))
coins = ([b[1] for b in bags if b[0].lower().startswith('coin')] + [None])[0]

if args:
	if bet:
		title = f'{name} raises their bet.'
	else:
		title = f'{name} places a bet.'

	if coins:
		for coin in coins.keys():	# convert 10cp => 10 cp for split
			args=args.replace(coin,f' {coin}')
		args=args.split()
		if len(args)==1:	# default select most owned coin
			auto_coin=([None]+list(bet.keys()))[-1]
			if not auto_coin:
				auto_coin=[c for c,v in coins.items() if v==max(coins.values())][0]
			args.append(auto_coin)

		coin_idx={c:args.index(c)-1 for c in coins.keys() if c in args}
		new_bet={c:int(args[i]) for c,i in coin_idx.items() if i>=0 and args[i].isdecimal()}
		if not new_bet:
			return f'echo Illegal bet. Use `{syntax}`'
		# TODO: check and update pouch
		for c,q in new_bet.items():
			have=coins.get(c,0)
			if have<q:
				return f'echo You do not have {q} {c}, but only {have}.'
			coins[c]=have-q
		character().set_cvar('bags',dump_json(bags))
	else:
		args="&*&"
		chips='chips'
		if not args.isdecimal():
			return f'echo Since you have no `!coins` pouch you can only bet a number of {chips} `{base_cmd} {game_name} [<amount>]'
		new_bet={chips:int(args)}

	# persist
	if bet:
		desc=f':heavy_plus_sign: {", ".join(f"{q} {c}" for c,q in new_bet.items())}\n'
	for c,v in new_bet.items():
		bet[c]=bet.get(c,0)+v
	state[game_name]=bet
	character().set_cvar(cvar,dump_json(state))

else:
	title= f'{name} counts {get("their","their")} bet.'
	new_bet=None


desc+=f':coin: {", ".join(f"{q} {c}" for c,q in bet.items())}' if bet else 'Bet: *None*'
if coins:
	desc+=f'\n:moneybag: {", ".join(f"{q} {c}" for c,q in coins.items() if q!=0)}'

return f'embed -title "{title}" -desc "{desc}" -color {color} -footer "{syntax} {base_cmd} drop."'
</drac2>