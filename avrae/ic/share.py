<drac2>
UVAR_SUBSCRIBED = "npc_subscribed_rosters"
SVAR_SERVER_ROSTERS = "npc_server_npcs"
ids=&ARGS&

if not ids:
	ids=load_json(npc_subscribed_rosters) if uvar_exists(UVAR_SUBSCRIBED) else []

if not ids:
	return f'echo Use `{ctx.prefix}{ctx.alias} share [<gvar>][...]`. If you do not provide gvars your personally subscribed gvars will be shared, but you have none.'

server_roster_gvars = load_json(get_svar(SVAR_SERVER_ROSTERS,'[]'))

# append new ids
server_roster_gvars+=[id for id in ids if id not in server_roster_gvars]

return f'svar {SVAR_SERVER_ROSTERS} {dump_json(server_roster_gvars)}'
</drac2>