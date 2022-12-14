<drac2>
# load the appropriate !play state for the player
uvar='Play_er'
player_state=load_json(get(uvar,'{}'))
coins=None
purse = None
default_image='none'
if player:=player_state.get('player'):
	state=player_state.setdefault(player,{})
	coins=state.get('coins')
	bags=None
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
	cvar = 'Play'
	player_state=None
	state=load_json(get(cvar, '{}'))
	player = name

	# get !coins pouch if any
	if (bags:=get('bags')) is not None:
		bags = load_json(bags)
		coins = ([b[1] for b in bags if b[0].lower().startswith('coin')] + [None])[0]
	purse=character().coinpurse

game_name='bet'
bet=state.setdefault(game_name,{})

base_cmd=f'{ctx.prefix}{ctx.alias}'
syntax=f'{base_cmd} collect [<winnings> [<coin>]]'

###  parse arguments for profit (same syntax as bet)
args="&*&".lower()
# inject spaces between numbers and alphas
args="".join(' '+c if i>0 and c.isalpha() and args[i-1].isdecimal() else c for i,c in enumerate(args) )
# convert it into a item:amount dict for arguments that are subsequent decimal,alpha
arg_list=args.split()
winnings=dict()
amount=None
for i,arg in enumerate(arg_list):
	if arg.isdecimal():
		amount=int(arg)
	if arg.isalpha() and amount is not None:
		winnings[arg]=winnings.get(arg, 0)+int(amount)
		amount=None
# last amount is with default coin: 1) current bet coin 2) most owned coin 3) chips
chips = 'chips'
if amount is not None:
	if bet:
		auto_coin = list(bet.keys())[-1]
	elif coins:
		auto_coin = ([c.lower() for c, v in coins.items() if v == max(coins.values())]+[chips])[0]
	else:
		auto_coin=chips
	winnings[auto_coin]=winnings.get(auto_coin, 0)+amount

# since we're collecting, the npc receives a coin pouch made up of the bet and collected coins
if player_state and not coins:
	coins = state.setdefault('coins',{})
	coins.update({coin:0 for coin in bet})
	coins.update({coin:0 for coin in winnings})

desc=f':coin: {", ".join(f"{q} {c}" for c,q in bet.items()) if bet else "Nothing"}'
desc+=f'\n:heavy_plus_sign: {", ".join(f"{q} {c}" for c,q in winnings.items())}' if winnings else ''


# update and persist !coins
collect=bet
collect.update({k:collect.get(k,0)+winnings.get(k,0) for k in winnings.keys()})
if coins:
	for c,q in coins.items():
		coins[c]=q + collect.get(c,0)
	desc+=f'\n:moneybag: {", ".join(f"{q} {c}" for c,q in coins.items() if q!=0)}'
	if bags:
		character().set_cvar('bags',dump_json(bags))
elif purse:
	purse.modify_coins(pp=collect.get('pp',0), gp=collect.get('gp',0), ep=collect.get('ep',0), sp=collect.get('sp',0), cp=collect.get('cp',0) )
	desc+='\n:moneybag: '+purse.compact_str()

# persist bet
state.pop(game_name)
if player_state is not None:
	set_uvar(uvar, dump_json(player_state))
else:
	character().set_cvar(cvar,dump_json(state))

return f'embed -title "{player} wins the bet." -desc "{desc}" -color {color} -thumb {image} -footer "{syntax}"'
</drac2>