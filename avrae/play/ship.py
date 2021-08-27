<drac2>
# load the appropriate !play state for the player
uvar='Play_er'
cvar='Play'
default_image='https://upload.wikimedia.org/wikipedia/commons/9/99/Dice_-_1-2-4-5-6.jpg'
player_state=load_json(get(uvar,'{}'))
if player:=player_state.get('player'):
	state=player_state.setdefault(player,{})
	# load npc rosters if any
	combined_roster=dict()
	for gvar in  load_json(get_svar("npc_server_npcs", '[]'))+load_json(get("npc_subscribed_rosters","[]")):
		combined_roster.update(load_json(gvar_data))
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
	image=image or default_image

# get the state for this game out of the player's game state
game_name='ship'
game=state.setdefault(game_name,{})

args="&*&"
turns=3
syntax=f'{ctx.prefix}{ctx.alias} {game_name}'

turn=game.get('turn',0)
bank=game.get('bank',[])
if turn>=turns:
	turn=0
	bank=[]

# roll
rolls=[roll('1d6') for _ in range(5-len(bank))]
target=[6,5,4]
for t in target:
	if t not in bank:
		if t in rolls:
			rolls.remove(t)
			bank.append(t)
		else:
			break

# update the game state
if done:= all(t in bank for t in target):
	game.clear()
else:
	turn+=1
	game['turn'] = turn
	game['bank'] = bank

# persist either the player's state or the character's state
if player_state is not None:
	set_uvar(uvar, dump_json(player_state))
else:
	character().set_cvar(cvar,dump_json(state))

# display game
ship=f'**{", ".join(str(r) for r in bank)}**, ' if bank else ''
cargo=", ".join(str(r) for r in rolls)
# output
if done:
	desc = f':sailboat: {ship} {cargo} = {sum(rolls)}'
else:
	desc= f'**Round:** {turn}/{turns} **Dice:** {ship} {cargo}'
	if turn==turns:
		desc+=' = 0\nNo more tries left.'
	else:
		desc+=f'\nTry again with `{syntax}`'

return f'embed -title "{player} rolls Ship, Captain and Crew." -desc "{desc}" -thumb {image} -footer "{syntax}" -color {color}'
</drac2>
