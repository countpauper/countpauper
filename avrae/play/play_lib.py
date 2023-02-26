uvar = 'Play_er'
cvar = 'Play'
player_state=None

def load_state(title, name, color, image):
	player_state = load_json(get(uvar, '{}'))
	if player := player_state.get('player'):
		# load npc rosters if any
		combined_roster = dict()
		for gvar in load_json(get_svar("npc_server_npcs", '[]')) + load_json(get("npc_subscribed_rosters", "[]")):
			combined_roster.update(load_json(get_gvar(gvar)))
		combined_roster.update(load_json(get("npc_local_roster", "{}")))
		# get the npc properties for the embed
		npc = combined_roster.get(player, {})
		return dict(
			storage=player_state,
			state = player_state.setdefault(player, {}),
			player=npc.get('name',player.capitalize()),
			embed=f'''-title "{npc.get('name',player.capitalize())} plays {title}" -color {npc.get('color', 0)} -thumb {npc.get('image', default_image)}'''
		)
	else:
		return dict(storage=None,
					state = load_json(get(cvar, '{}')),
					player = name,
					embed=f'''-title "{name} plays {title}" -color {color} -thumb {image}''')

def save_state(state):
	if player_state is not None:
		set_uvar(uvar, dump_json(player_state))
	else:
		character().set_cvar(cvar, dump_json(state))
