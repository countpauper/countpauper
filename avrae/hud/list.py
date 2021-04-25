<drac2>
config=load_json(get_gvar('f9fd35a8-1c8e-477c-b66e-2eeee09a4735'))
display_fields={
	None:["name","hp","ac","hd","spell","xp","xp","class"],
	"Consumable":["cc","flag"],
	"Item":["item"]
}
uvar_name='default_hud_items'
cvar_name='hud_items'

selected_items=load_json(get(cvar_name,get(uvar_name,'[]')))

fields=[]
desc=""
sep=', '
for field,display in display_fields.items():
	items=[f'{props.get("icon","")} {f"**{item}**" if item in selected_items else item}' for item,props in config.items() if props.get('display') in display]
	if field and items:
		fields.append(f'-f "{field}|{sep.join(items)}"')
	else:
		desc=f'-desc "{sep.join(items)}"'
footer=f'-footer "Use {ctx.prefix}{ctx.alias} [set|var] <item>[...] [-i] [-t "<target>"][...] default auto -cc(<counter>[,<icon>[,flag]])"'
return f'embed -title "{ctx.alias} supports the following items" {desc} {" ".join(fields)} {footer} -color {color}'
</drac2>