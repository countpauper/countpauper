embed <drac2>
# TODO https://discord.com/channels/688962463860785242/708772692022788157/816839465820487773
# - make an npc version
# - works mostly the same but uses the npc roster
# - extra category for set
# - category filter for list
# - default color for npcs
# - scan gvars for additional chars
# - move npcs and categories to specific gvars and

settings=load_json(get_svar('ic_settings','{}'))
if footer_format:=settings.get('footer'):
	vars=[]
	for idx,c in enumerate(footer_format):
		if c!='{':
			continue
		remaining=footer_format[idx+1:]
		if not '}' in remaining:
			continue
		vars.append(remaining[:remaining.index('}')])
	footer=f'-footer "{footer_format}"'
	for var in vars:
		footer=footer.replace('{'+var+'}',str(get(var,'')))
else:
	footer=''

arg_str = """&*&"""

options=('-image','-color','-name','-thumb','-footer')
option_idx=[arg_str.find(option) for option in options]
if options_start:=[idx for idx in option_idx if idx>=0]:
	options_start=min(options_start)
	option_str=arg_str[options_start:]
	arg_str=arg_str[:options_start]
else:
	option_str=''

# activate the first argument
active_var='sayActive'
roster=load_json(get('userRoster','[]'))
args=&ARGS&
if args:
	select=args[0].lower()
	# match only id, full or partial to prevent accidental selection when saying your own name.
	match = [u.d for u in roster if u.d.lower() == select]
	match += [u.d for u in roster if u.d.lower().startswith(select)]
	if match:
		character().set_cvar(active_var,match[0])
		arg_str=arg_str[len(select)+1:]

# get user from roster, default to current character name and portrait,  random color
active=get(active_var,name).lower()
user=[u for u in roster if u.d.lower()==active]+[dict(d=active,n=name,i=image,c=get('color'))]
user=user[0]

# override user with arguments
args=argparse(option_str)
title = args.last('name', user.n)
thumb = args.last('thumb', user.i)
color = args.last('color', user.c)
img = args.last('image')
# TODO: override footer? in which order?

palette = load_json(get_gvar('74f3bd10-7163-4838-ad27-344ad0fb6b83'))
color=palette.get(color.lower(),color)

txt=''
openquote=False
openitalic=False
for c in arg_str:
	if c=='"':
		txt+='“”'[openquote]
		openquote = not openquote
	elif c=='*':
		txt+=c
		openitalic = not openitalic
		# TODO: track open cursive and spoilers, autoclose when mixing
	else:
		txt+=c

if openquote:	# actually can't happen because avrae is typing ...
	txt+='”'

if openitalic:
	txt+='*'

return f'-title "{title}" -desc "{txt}" {footer} -thumb {thumb} {f"-color {color}" if color is not None else ""} {f"-image {img}" if img else ""}'
</drac2>