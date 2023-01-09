<drac2>
# load the appropriate !play state for the player
uvar='Play_er'
cvar='Play'
default_image='https://cdn.discordapp.com/attachments/751098635038621868/944958030573867008/pigs1.png'
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
	image = image or default_image

# get the game state
game_name='pig'
pig=state.setdefault(game_name,[[]])

args="&*&".lower()
base_cmd=f'{ctx.prefix}{ctx.alias}'
base_syntax=f'{base_cmd} {game_name}'
syntax=f'{base_syntax} [<dice>|<hold>|<reset>]'

embed_stuff=f'-footer "{syntax}" -color {color} -thumb {image}'

over=False
desc=f"Round  `{len(pig)}` - "
if args in ['hold','stand']:
	over=True
	desc+=" Hold"
	if not pig[-1]:
		pig[-1].append(0)
elif args in ['reset','restart']:
	pig=[]
	over = True
	desc='**Game restarted**'
else:
	new_roll=vroll(args or '1d6')
	desc+=f"Roll: {new_roll}."
	pig[-1].append(new_roll.total)
	if new_roll.total==1:
		pig[-1]=[0]
		over=True
		desc+="\nGreedy Pig!"
if over:
	pig.append([])
	if len(pig)>5 or len(pig)<=1:
		desc+="\n**Game Over.**"
	else:
		desc+="\nNext player."
else:
	desc+=f"\nRoll again (`{base_syntax} {args}`) or hold (`{base_syntax} hold`) ?"


# persist either the player's state or the character's state
if len(pig)>5:
	state.pop(game_name)
else:
	state[game_name]=pig

if player_state is not None:
	set_uvar(uvar, dump_json(player_state))
else:
	character().set_cvar(cvar,dump_json(state))
rounds=[sum(rolls) for rolls in pig if rolls]
if rounds:
	score=f'-f "Score|{" + ".join(str(r) for r in rounds)} = **{sum(rounds)}**"'
else:
	score=''

# output
return f'embed -title "{player} plays Greedy Pig." -desc "{desc}" {score} {embed_stuff}'
</drac2>

