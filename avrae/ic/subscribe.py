embed <drac2>
UVAR_SUBSCRIBED = "npc_subscribed_rosters"
subscribed_roster_gvars = load_json(npc_subscribed_rosters) if uvar_exists(UVAR_SUBSCRIBED) else []
ids=&ARGS&
if not ids:
	ids=subscribed_roster_gvars	 # dump current
else:	# update
	invalid = []
	for id in ids:
		id_parts = id.split('-')
		if not [len(idp) for idp in id_parts] == [8, 4, 4, 4, 12] or not all(idp.isalnum() for idp in id_parts):
			invalid.append(f'`{id}`')
	if invalid:
		return err(f'The following gvar identifiers are not properly formatted: {",".join(invalid)}.')

	subscribed_roster_gvars+=ids
	set_uvar(UVAR_SUBSCRIBED, dump_json(subscribed_roster_gvars))
if not ids:
	ids=["None"]
else:
	ids=[f'[`{id}`](https://avrae.io/dashboard/gvars?lookup={id})' for id in ids]
</drac2>
-title "Subscribed to the roster{{'s' if len(ids)>1 else ''}} at:" -desc "{{'\n'.join(ids)}}"
