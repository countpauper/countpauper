<drac2>
# TODO https://discord.com/channels/688962463860785242/708772692022788157/816839465820487773
# - make an npc version
# - works mostly the same but uses the npc roster
# - extra category for set
# - category filter for list
# - default color for npcs
# - scan gvars for additional chars
# - move npcs and categories to specific gvars and

settings=load_json(get_svar('ic_settings','{}'))

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
footer = args.last('footer',settings.get('footer',''))

# Check location limitation. cvar configured in settings.location, encoded as , separate list or json
if loc_var:=settings.get('location'):
	if loc:=get(loc_var):
		loc=loc.strip()
		if loc[0] in '[{':
			loc=load_json(loc)
		else:
			loc=loc.split(',')
		loc=[l.lower() for l in loc]
		if ctx.channel.name not in loc and str(ctx.channel.id) not in loc:
			return f'techo 8 {title} is not currently in this channel.'
channel=ctx.channel.name	# set channel to be used in footer

# convert color names to hex
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

# define words for experience (footer)
words = len(txt.split())
cc = 'Experience'
if ch:=character():
	xp=settings.get('xp','0')
	# find vars in xp expression
	xp_vars=[]
	for idx,c in enumerate(xp):
		if c!='{':
			continue
		remaining=xp[idx+1:]
		if not '}' in remaining:
			continue
		xp_vars.append(remaining[:remaining.index('}')])
	for var in xp_vars:
		xp=xp.replace('{'+var+'}',str(get(var,'0')))
	# roll and add the xp
	if xp:=roll(xp):
		ch.mod_cc(cc,xp)

# convert footer configuration to footer text
if footer:
	# find vars in footer expression
	vars=[]
	footer=footer.strip('"')
	for idx,c in enumerate(footer):
		if c!='{':
			continue
		remaining=footer[idx+1:]
		if not '}' in remaining:
			continue
		vars.append(remaining[:remaining.index('}')])

	for var in vars:
		footer=footer.replace('{'+var+'}',str(get(var,'' if not ch or not ch.cc_exists(var) else ch.cc_str(var))))
	footer=f'-footer "{footer}"'
return f'embed -title "{title}" -desc "{txt}" {footer} -thumb {thumb} {f"-color {color}" if color is not None else ""} {f"-image {img}" if img else ""}'
</drac2>