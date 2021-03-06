<drac2>
roster_var='npc_local_roster'
roster_gvar='npc_subscribed_rosters'
roster_svar='npc_shared_roster'
active_var='npc_global_id'

roster={}
for rgv in load_json(get(roster_gvar,'[]')):
	roster.update(load_json(get_gvar(rgv)))
local_roster=load_json(get(roster_var, '{}'))
roster.update(local_roster)
roster.update(load_json(get_svar(roster_svar,'{}')))

# select groups filtered by arguments if any
args=&ARGS&
if args:
	groups={a for a in args}
else:
	groups={ c.get('group') for c in roster.values() }

# show the whole roster in a table embed
active = get(active_var, '').lower()
footer= '-footer "* active, + local [$total]"'

header=f'`  {"Id":<12} {"Name":<24} {"Color":<7} Thumbnail`\n'
# group
chars={}
for group in groups:
	chars[group]=[f'`{"*" if id.lower()==active else " "}{"+" if id in local_roster.keys() else " "}{id:<12} {c.name[:24]:<24} {c.color if c.color else "none":<8}` {f"[link]({c.image})" if c.image else "[none]"}'
				  for id,c in roster.items() if c.get('group')==group]

# limit size
embed_limit=6000-50-len(header)-len(footer) # maximum embed command minus overhead
npcs=0
shown=0
for group, char_list in chars.items():
	embed_limit-=(len(group) if group else 0)+len(' -f"|"\n')
	npcs+=len(char_list)
	section_limit=2040
	for l,char in enumerate(char_list):
		embed_limit-=len(char)+2
		section_limit-=len(char)+2
		if embed_limit<=0 or section_limit<=0:
			char_list=char_list[:l]
			break
	chars[group]=char_list
	shown+=len(char_list)

footer=footer.replace('$total',f'{shown}/{npcs}' if shown<npcs else str(npcs))

nl='\n'
fields=[f'-desc "{header}{nl.join(chars[None]) if None in chars.keys() else ""}"']
fields+=[f'-f "{group}|{nl.join(c) if (c:=chars[group]) else "None"}"' for group in chars.keys() if group is not None]
fields=nl.join(fields)

if len(fields)+len(footer)+32>6000:
	return f'echo Too long {len(fields)} {embed_limit}'
return f'embed -title Characters {fields} {footer}'
</drac2>
