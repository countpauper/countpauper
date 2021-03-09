embed <drac2>
# TODO: update help -group, default name, color and palette
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
    "sayimport",
    "group",
    "share"
]

out = []
args = &ARGS&
if len(args) == 0:
    return '-title "No arguments provided; see `!help npc edit`"'
npc_id = args[0]
if npc_id in RESERVED_WORDS:
    out.append(f'-title "Failed to edit an NPC!"')
    out.append(f'-desc "The NPC ID matches one used by a command..."')
else:
    parsed = argparse(args[1:])
    npc = local_roster[npc_id] if npc_id in local_roster else {
        "name": npc_id.title(), "image": "", "color": get('color')}

    if new_name := parsed.last("name"):
        npc["name"] = new_name
    if new_image := parsed.last("image"):
        npc["image"] = new_image
    if new_color := parsed.last("color"):
        palette = load_json(get_gvar('74f3bd10-7163-4838-ad27-344ad0fb6b83'))
        new_color = palette.get(new_color.lower(), new_color)
        npc["color"] = new_color
    if new_group := parsed.last("group"):
        if new_group.lower() in ["reset","-"]:
            if "group" in npc:
                npc.pop("group")
        else:
            npc["group"] = new_group

    local_roster[npc_id] = npc
    set_uvar(UVAR_LOCAL, dump_json(local_roster))

    out.append(f'-title "Edited an NPC!"')
    out.append(f'-f "ID|{npc_id}"')
    if npc_name:=npc["name"]:
        out.append(f'-f "Name|{npc_name}"')
    if npc_image:=npc["image"]:
        out.append(f'-thumb {npc_image}')
        out.append(f'-f "Image|[link]({npc_image})"')
    if npc_color:=npc["color"]:
        out.append(f'-f "Color|{npc_color}" -color {npc_color}')
    if npc_group:=npc.get("group"):
        out.append(f'-f "Group|{npc_group}"')
    else:
        out.append(f'-f Group|None')

return " ".join(out)
</drac2>