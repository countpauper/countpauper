<drac2>
# load the appropriate !play state for the player
uvar='Play_er'
cvar='Play'
player_state=load_json(get(uvar,'{}'))
default_image='https://cdn.discordapp.com/attachments/751098635038621868/825619306037248040/3041-DZ.png'
if player:=player_state.get('player'):
	state=player_state.setdefault(player,{})
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
	image = image or default_image

# get the game state
game_name='hoard'
hoard=state.setdefault(game_name,{})

args="&*&"
base_cmd=f'{ctx.prefix}{ctx.alias}'
base_syntax=f'{base_cmd} {game_name}'
syntax=f'{base_syntax} [<dice>|<number eaten>]'

# play first roll
if not args:
	if not hoard:
		args='3d6'
	else:	# reveal
		desc=f""":dragon: Hoard : {", ".join(f"*{d}*" if d>0 else f"~~{-d}~~" for d in hoard)} = **{sum(d for d in hoard if d>0)}**
		The player with the biggest hoard wins. Split the pot on ties."""
		hoard=None

embed_stuff=f'-footer "{syntax}" -color {color} -thumb {image}'
if args:
	# plunder
	if args.isdigit():
		args=int(args)
		hoard=[-args if h==args else h for h in hoard]
		desc=f""":dragon: Hoard plundered [{args}] ||{", ".join(f"*{d}*" if d>0 else f"~~{-d}~~" for d in hoard)}||. **Only {player}  can peek!**
		Raise your bet with `{base_cmd} bet <amount>` or drop out with `{base_cmd} drop`. Then reveal with `{base_syntax}`"""
	# roll plunder die
	else:
		dice=args.split('d',maxsplit=1)
		if not dice[0]:
			dice[0]=1
		elif dice[0].isdigit():
			dice[0]=int(dice[0])
		else:
			return f'echo `{syntax}`'
		if len(dice)<2:
			return f'echo `{syntax}`'

		roll_str='1d'+dice[1]
		if dice[0]==1:
			v=vroll(roll_str)
			desc=f""":dragon: Hoard Plundered {v}
			All dragon players must run `{base_syntax} {v.total}`"""
			return f'embed -title "{ctx.author.display_name} plunders the hoard." -desc "{desc}" -color 1090FF -thumb {default_image}'
		else:	# roll for hoard
			hoard=[roll(roll_str) for _ in range(dice[0])]
			desc=f"""{player} :dragon: hoard. ||{", ".join(f"*{d}*" for d in hoard)}||. **Only {player} can peek!**
			Place your bet with `{base_cmd} bet <amount>`. 
			Then the dealer can plunder with `{base_syntax} d{dice[1]}`"""

# persist either the player's state or the character's state
if hoard is None:
	state.pop(game_name)
else:
	state[game_name]=hoard
if player_state is not None:
	set_uvar(uvar, dump_json(player_state))
else:
	character().set_cvar(cvar,dump_json(state))

# output
return f'embed -title "{player} plays Dragon Hoard." -desc "{desc}" {embed_stuff}'
</drac2>

