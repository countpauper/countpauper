<drac2>
uvar='Play_er'
cvar='Play'
player_state=load_json(get(uvar,'{}'))
coins=None
default_image='none'
if player:=player_state.get('player'):
	state=player_state.setdefault(player,{})
	bags=None
	purse=None
	coins=state.get('coins',coins)
	their = 'their'
	# load npc rosters if any
	combined_roster=dict()
	for gvar in  load_json(get_svar("npc_server_npcs", '[]'))+load_json(get("npc_subscribed_rosters","[]")):
		combined_roster.update(load_json(get_gvar(gvar)))
	combined_roster.update(load_json(get("npc_local_roster","{}")))
	# get the npc properties for the embed
	npc=combined_roster.get(player,{})
	player=npc.get('name',player.capitalize())
	image=npc.get('image',default_image)
	color=npc.get('color',0)
else:
	player_state=None
	state=load_json(get(cvar, '{}'))
	player = name
	# get !coins pouch if any
	if (bags:=get('bags')) is not None:
		bags = load_json(bags)
		coins = ([b[1] for b in bags if b[0].lower().startswith('coin')] + [None])[0]
	if not coins:
		purse=character().coinpurse
	their=get("their","their")

game_name='bet'
bet=state.setdefault(game_name,{})

base_cmd=f'{ctx.prefix}{ctx.alias}'
syntax=f'{base_cmd} {game_name} [<amount> [<coin>]]'

desc = ''
### Parse arguments
args = "&*&".lower()
# inject spaces between numbers and alphas
args="".join(' '+c if i>0 and c.isalpha() and args[i-1].isdecimal() else c for i,c in enumerate(args) )

# convert it into a item:amount dict for arguments that are subsequent decimal,alpha
arg_list=args.split()
new_bet=dict()
amount=None
for i,arg in enumerate(arg_list):
	if arg.isdecimal():
		amount=int(arg)
	if arg.isalpha() and amount is not None:
		new_bet[arg]=new_bet.get(arg, 0)+int(amount)
		amount=None
# convert single letter coins to coins
translate=dict(c='cp',s='sp',e='ep',g='gp')
for org,targ in translate.items():
	if  org in new_bet:
		new_bet[targ] = new_bet.pop(org)
# remove small keys that are not coins:
coin_names=['cp','sp','ep','gp','pp']
new_bet={k:a for k,a in new_bet.items() if len(k)>2 or k in coin_names}

# last amount is with default coin: 1) current bet coin 2) most owned coin 3) chips
chips = 'chips'
if amount is not None:
	if bet:
		auto_coin = list(bet.keys())[-1]
	elif coins:
		auto_coin = ([c.lower() for c, v in coins.items() if v == max(coins.values())]+[chips])[0]
	elif purse:
		coin_values={c:purse.get_coins().get(c,0) for c in coin_names}
		auto_coin = ([c for c,v in coin_values.items() if v==max(coin_values.values())]+[chips])[0]
	else:
		auto_coin=chips
	new_bet[auto_coin]=new_bet.get(auto_coin, 0)+amount

if new_bet:
	if bet:
		title = f'{player} raises {their} bet.'
	else:
		title = f'{player} places a bet.'

	if coins:
		for c,q in new_bet.items():
			if (have:=coins.get(c)) is not None:
				if have<q:
					return f'echo You do not have {q} {c}, but only {have}.'
				coins[c]=have-q
		if bags:
			character().set_cvar('bags',dump_json(bags))
	elif purse:
		purse.modify_coins(pp=-new_bet.get('pp',0), gp=-new_bet.get('gp',0), ep=-new_bet.get('ep',0), sp=-new_bet.get('sp',0), cp=-new_bet.get('cp',0) )

	# persist
	if bet:
		desc=f':heavy_plus_sign: {", ".join(f"{q} {c}" for c,q in new_bet.items())}\n'
	for c,v in new_bet.items():
		bet[c]=bet.get(c,0)+v
	if player_state is not None:
		set_uvar(uvar, dump_json(player_state))
	else:
		character().set_cvar(cvar, dump_json(state))

else:
	title= f'{player} counts {their} bet.'
	new_bet=None

desc+=f':coin: {", ".join(f"{q} {c}" for c,q in bet.items())}' if bet else 'Bet: *None*'
if coins:
	desc+=f'\n:moneybag: {", ".join(f"{q} {c}" for c,q in coins.items() if q!=0)}'

return f'embed -title "{title}" -desc "{desc}" -color {color} -footer "{syntax} {base_cmd} drop." -thumb {image}'
</drac2>