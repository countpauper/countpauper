tembed <drac2>
UVAR_LOCAL = "npc_local_roster"
local_roster = load_json(npc_local_roster) if uvar_exists(UVAR_LOCAL) else {}

RESERVED_WORDS = [
    "subscribe",
    "unsubscribe",
    "edit",
    "delete",
    "list",
    "setglobal",
    "setchannel",
    "reset",
    "dump",
    "sayimport"
]

out = []
args = & ARGS &
if len(args) == 0:
    return '-title "No arguments provided; see `!help npc edit`"'
npc_id = args[0]
if npc_id in RESERVED_WORDS:
    out.append(f'-title "Failed to edit an NPC!"')
    out.append(f'-desc "The NPC ID matches one used by a command..."')
else:
    parsed = argparse(args[1:])
    npc = local_roster[npc_id] if npc_id in local_roster else {
        "name": "Give me a name!", "image": "", "color": ""}

    new_name = parsed.last("name")
    new_image = parsed.last("image")
    new_color = parsed.last("color")

    if new_name is not None:
        npc["name"] = new_name
    if new_image is not None:
        npc["image"] = new_image
    if new_color is not None:
        npc["color"] = new_color

    local_roster[npc_id] = npc
    set_uvar(UVAR_LOCAL, dump_json(local_roster))

    out.append(f'-title "Edited an NPC!"')
    out.append(f'-f "ID|{npc_id}"')
    if (npc["name"]):
        out.append(f'-f "Name|{npc["name"]}"')
    if (npc["image"]):
        out.append(f'-thumb {npc["image"]}')
        out.append(f'-f "Image|{npc["image"]}"')
        thumb = npc["image"]
    if (npc["color"]):
        npc_color = npc["color"]
        out.append(f'-f "Color|{npc["color"]}"')

return " ".join(out)
</drac2>