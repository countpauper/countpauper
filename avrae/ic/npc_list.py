<drac2>
UVAR_LOCAL='npc_local_roster'
UVAR_SUBSCRIBED='npc_subscribed_rosters'
SVAR_SERVER_ROSTERS = "npc_server_npcs"
UVAR_GLOBAL_NPC='npc_global_id'
UVAR_CHANNEL_BINDINGS = "npc_channel_bindings"
GROUP_KEY='group'

# load the rosters
roster={}
for gvar in load_json(get_svar(SVAR_SERVER_ROSTERS,'[]')):
	if gv:=get_gvar(gvar):
		roster.update(load_json(gv))
	else:
		err(f'Server gvar `{gvar}` is not accessible.')
for gvar in load_json(get(UVAR_SUBSCRIBED,'[]')):
	if gv:= get_gvar(gvar):
		roster.update(load_json(gv))
	else:
		err(f'Subscribed gvar `{gvar}` is not accessible.')

local_roster=load_json(get(UVAR_LOCAL, '{}'))
roster.update(local_roster)

# parse options
args=&ARGS&
COMPACT_OPTION='-c'
LINK_OPTION='-l'
NARROW_OPTION='-n'
if compact:=COMPACT_OPTION in args:
	args.remove(COMPACT_OPTION)
if link:=LINK_OPTION in args:
	args.remove(LINK_OPTION)
if narrow:=NARROW_OPTION in args:
	args.remove(NARROW_OPTION)
# select groups filtered by arguments if any
none_group=False
if args:
	groups={a for a in args}
else:
	groups={ c.get(GROUP_KEY) for c in roster.values() }
	if none_group:=(None in groups):
		groups.remove(None)

groups=list(groups)
groups.sort()
if none_group:
	groups=[None]+groups

# select the active character if any
channel_bindings = load_json(get(UVAR_CHANNEL_BINDINGS,'{}'))
active = channel_bindings.get(str(ctx.channel.id),get(UVAR_GLOBAL_NPC, '')).lower()

# show the whole roster in a table embed by group
chars={}
cid,cname,ccol = (8,16,6) if narrow else (16,32,8)

if compact:
	for group in groups:
		chars[group]=[f'{"*" if id.lower()==active else ""}{"†" if id in local_roster.keys() else ""}{id}' for id,c in roster.items() if c.get(GROUP_KEY)==group]
else:
	for group in groups:
		chars[group]=[]
		for id,c in roster.items():
			if c.get(GROUP_KEY)==group:
				if link:
					img = f'[`link `]({c.image})' if c.image else '`none `'
				else:
					img=f':white_check_mark:' if c.image else ':x:'

				chars[group].append(f'`{"*" if id.lower() == active else " "}{"†" if id in local_roster.keys() else " "}` `{id[:cid]:<{cid}}` `{c.name[:cname]:<{cname}}` `{c.color[:ccol] if c.color else "none":<{ccol}}` {img}')

footer= '-footer "* active, † local, listed $total"'
if compact:
	header=''
else:
	header = f'**`  ` `{"Id":<{cid}}` `{"Name":<{cname}}` `{"Color":<{ccol}}` `Thumb`**\n'

# truncate char lists and groups, limited by embed size, desc limit and field size and count limit
title='-title NPCs'
groups_field='Groups'
embed_limit=6000-3-len(title)-len(groups_field)-len(footer) # maximum embed command minus overhead and safety margin
# subtract all group names, because they'll be in the unlisted groups anyway
embed_limit-=sum(len(g)+1 for g in chars.keys() if g is not None)

npcs=0
shown=0
fields=24
unlisted_groups=[]
for group, char_list in chars.items():
	field = group is not None
	group_overhead=len('-f "|"…' ) if field else len('-desc ""')+len(header)
	embed_limit-=(group_overhead)
	npcs+=len(char_list)
	section_limit=(1024 if field else 2048)-group_overhead
	unlisted=False
	for l,char in enumerate(char_list):
		embed_limit-=len(char)+2
		section_limit-=len(char)+2
		if embed_limit<=0 or section_limit<=0:
			char_list=char_list[:l]
			unlisted=len(char_list)==0
			char_list.append('…')
			break
	fields-=1 if field else 0
	if fields<=0 or unlisted:
		unlisted_groups.append(group)
	else:
		shown+=len(char_list)
	chars[group] = char_list

footer=footer.replace('$total',f'{shown}/{npcs}' if shown<npcs else str(npcs))

nl='\n'
sep=', ' if compact else nl
fields=[f'-desc "{header}{sep.join(chars[None]) if None in chars.keys() else ""}"']
fields+=[f'-f "{group}|{sep.join(c) if (c:=chars[group]) else "None"}"' for group in chars.keys() if group is not None and group not in unlisted_groups]
if unlisted_groups:
	fields.append(f'-f "{groups_field}|{", ".join(g for g in unlisted_groups)}"')
fields=nl.join(fields)

output = f'embed {title} {fields} {footer}'
#if len(output)>6000:
#	return f'echo Too long {len(output)} {len(fields)} {embed_limit}'
return output
</drac2>
