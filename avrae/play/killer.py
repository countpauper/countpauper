<drac2>
# load the appropriate !play state for the player
uvar='Play_er'
cvar='Play'
default_image='https://cdn.discordapp.com/attachments/751098635038621868/867699902578032670/Kill_dice.png'
player_state=load_json(get(uvar,'{}'))
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
	image=image or default_image

# get the state for this game out of the player's game state
game_name='killer'
game=state.setdefault(game_name,[])

killer=1
turns=len(game_name)
syntax=f'{ctx.prefix}{ctx.alias} {game_name}'
args=&ARGS&

if len(game)>=turns or any(d==killer for d in game):
	game.clear()
rounds=1
all_arg='all'
if all_arg in args:
	args.remove(all_arg)
	rounds=turns-len(game)

rollstr=args[0] if args else '1d6'

# roll
for _ in range(rounds):
	game.append(roll(rollstr))
	if game[-1]==killer:
		break

# persist either the player's state or the character's state
if player_state is not None:
	set_uvar(uvar, dump_json(player_state))
else:
	character().set_cvar(cvar,dump_json(state))

turn=len(game)
if any(d==killer for d in game):
	result='Killed!'
else:
	result=f'Total {sum(game)}'
syntax='!play killer [die=d6] [all]'

desc="`K I L L E R`\n"
desc+="`" +" ".join(f"{d}" for i,d in enumerate(game))+"`"
desc +=f'\n**{result}**'
return f'embed -title "{player} rolls the {rollstr}" -desc "{desc}" -thumb {image} -footer "{syntax}" -color {color}'
</drac2>
