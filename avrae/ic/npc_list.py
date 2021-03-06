<drac2>
# TODO: test limiter
# -c compact mode (only ids)
#   remove header on combat mode
#   , separate on combat mode
# document use of svars on workshop

UVAR_LOCAL='npc_local_roster'
UVAR_SUBSCRIBED='npc_subscribed_rosters'
SVAR_SERVER_ROSTERS = "npc_server_npcs"
UVAR_GLOBAL_NPC='npc_global_id'
UVAR_CHANNEL_BINDINGS = "npc_channel_bindings"
GROUP_KEY='group'

# load the rosters
roster={}
for rgv in load_json(get(SVAR_SERVER_ROSTERS,'[]')):
	roster.update(load_json(get_gvar(rgv)))
for rgv in load_json(get(UVAR_SUBSCRIBED,'[]')):
	roster.update(load_json(get_gvar(rgv)))
local_roster=load_json(get(UVAR_LOCAL, '{}'))
roster.update(local_roster)

# parse options
args=&ARGS&
COMPACT_OPTION='-c'
if compact:=COMPACT_OPTION in args:
	args.remove(COMPACT_OPTION)

# select groups filtered by arguments if any
if args:
	groups={a for a in args}
else:
	groups={ c.get(GROUP_KEY) for c in roster.values() }

# select the active character if any
channel_bindings = load_json(get(UVAR_CHANNEL_BINDINGS,'{}'))
active = channel_bindings.get(str(ctx.channel.id),get(UVAR_GLOBAL_NPC, '')).lower()

# show the whole roster in a table embed by group
chars={}
if compact:
	for group in groups:
		chars[group]=[f'{"*" if id.lower()==active else ""}{"†" if id in local_roster.keys() else ""}{id}' for id,c in roster.items() if c.get(GROUP_KEY)==group]
else:
	for group in groups:
		chars[group]=[f'`{"*" if id.lower()==active else " "}{"†" if id in local_roster.keys() else " "}{id[:12]:<12} {c.name[:24]:<24} {c.color[:8]if c.color else "none":<8}` {f"[link]({c.image})" if c.image else "[none]"}'
					  for id,c in roster.items() if c.get(GROUP_KEY)==group]

footer= '-footer "* active, † local [$total]"'
header=f'`  {"Id":<12} {"Name":<24} {"Color":<7} Thumbnail`\n' if not compact else ''

# limit size
embed_limit=6000-50-len(header)-len(footer) # maximum embed command minus overhead and safety margin
npcs=0
shown=0
for group, char_list in chars.items():
	group_overhead=(len('"" ')+len(header) if group is None else len('-f "|"' )+len(group))
	embed_limit-=(group_overhead)
	npcs+=len(char_list)
	section_limit=2047-group_overhead
	for l,char in enumerate(char_list):
		embed_limit-=len(char)+2
		section_limit-=len(char)+2
		if embed_limit<=0 or section_limit<=0:
			char_list=char_list[:l]
			char_list.append('…')
			break
	chars[group]=char_list
	shown+=len(char_list)

footer=footer.replace('$total',f'{shown}/{npcs}' if shown<npcs else str(npcs))

nl='\n'
sep=', ' if compact else nl
fields=[f'-desc "{header}{sep.join(chars[None]) if None in chars.keys() else ""}"']
fields+=[f'-f "{group}|{sep.join(c) if (c:=chars[group]) else "None"}"' for group in chars.keys() if group is not None]
fields=nl.join(fields)

if len(fields)+len(footer)+32>6000:
	return f'echo Too long {len(fields)} {embed_limit}'
return f'embed -title Characters {fields} {footer}'
</drac2>
