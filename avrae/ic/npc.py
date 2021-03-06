<drac2>
UVAR_SUBSCRIBED = "npc_subscribed_rosters"
UVAR_LOCAL = "npc_local_roster"
UVAR_GLOBAL_NPC = "npc_global_id"
UVAR_CHANNEL_BINDINGS = "npc_channel_bindings"
SVAR_SERVER_ROSTERS = "npc_server_npcs"

# Argument Processing

# Data loading
server_roster_gvars = load_json(get_svar(SVAR_SERVER_ROSTERS,'[]'))
subscribed_roster_gvars = load_json(npc_subscribed_rosters) if uvar_exists(UVAR_SUBSCRIBED) else []
local_roster = load_json(npc_local_roster) if uvar_exists(UVAR_LOCAL) else {}

channel_bindings = load_json(npc_channel_bindings) if uvar_exists(UVAR_CHANNEL_BINDINGS) else {}

# NPC Roster Priority: User > Subscribed > Server rosters.
combined_roster = {}

# To build the NPC roster, first resolve all server rosters in order;
for gvar in server_roster_gvars:
    if gvar_data := get_gvar(gvar):
        combined_roster.update(load_json(gvar_data))
    else:
        err(f'Server gvar `{gvar}` is not accessible.')

# Then resolve subscribed rosters in order;
for gvar in subscribed_roster_gvars:
    if gvar_data := get_gvar(gvar):
        combined_roster.update(load_json(gvar_data))
    else:
        err(f'Subscribed gvar `{gvar}` is not accessible.')

# Then apply the user's local roster.
combined_roster.update(local_roster)

thumb = ""
npc_color = ""
npc = None
msg = None

out = ['embed']
argstring = '''&*&'''.replace('"', '\\"').replace("'", "\\'")
if not argstring:
    return f'techo 5 Use `{ctx.prefix}{ctx.alias} [<npc>] <text>`'

# find images by extension and remove them from the argstring text
args=argstring.split()
image_extensions=['.png','.gif','.jpg','.jpeg','.gifv']
if images:=[a for a in args if any(a.lower().endswith(ext) for ext in image_extensions)]:
    image=images[-1]
    argstring=argstring.replace(image,'')
else:
    image=None

cmd = "&1&"
channel_id = str(ctx.channel.id)
if cmd in combined_roster:
    npc = combined_roster[cmd]
    mindex = len(cmd) + 1
    msg = argstring[mindex:]
elif channel_id in channel_bindings:
    npc_id = channel_bindings[channel_id]
    if npc_id in combined_roster:
        npc = combined_roster[npc_id]
        msg = argstring
    else:
        return f'techo 10 Could not find the NPC bound to this channel: `{npc_id}`. Use `!npc setchannel reset` to remove the binding.'
elif uvar_exists(UVAR_GLOBAL_NPC) and npc_global_id in combined_roster:
    npc = combined_roster[npc_global_id]
    msg = argstring

# Couldn't find an NPC for the given ID...
else:
    return f'techo 6 Could not find NPC `{cmd}`. You can use ctrl-Z to recall your input, until you switch channels. This message will self destruct in 3… 2… 1…'

if npc:
    thumb = npc["image"]
    npc_color = npc["color"]
    out.append(f'-title "{npc["name"]}"')

if msg:
    out.append(f'-desc "{msg}"')

out.append(f'-color {npc_color if npc_color else color}')

if thumb:
    if image or msg:
        out.append(f'-thumb {thumb}')
    else:
        out.append(f'-image {thumb}')

if image:
    out.append(f'-image {image}')

return " ".join(out)
</drac2>