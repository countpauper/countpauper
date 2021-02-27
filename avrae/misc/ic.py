embed <drac2>
arg_str = """&*&"""

options=('-image','-color','-name','-thumb')
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

palette = {"black":"000000","grey":"808080","silver":"c0c0c0","white":"ffffff","red":"ff0000","maroon":"800000","yellow":"ffff00","olive":"808000","lime":"00ff00","green":"008000","aqua":"00ffff","teal":"008080","navy":"000080","blue":"0000ff","pink":"ff1493","purple":"800090","copper":"b87333","blood":"a42208","coral":"ff7f50","pinkish":"ffb6c1","orange":"ff5500","gold":"d4af37","chiffon":"fffacd","lavender":"e6e6fa","plum":"dda0dd","violet":"ee82ee","magenta":"ff00ff","orchid":"ba55d3","indigo":"4b0082","slate":"6a5acd","chartreuse":"7fff00","forest":"228b22","cyan":"00ffff","turquoise":"40e0d0","sky":"87ceeb","bisque":"ffe4c4","wheat":"f5deb3","tan":"d2b48c","sand":"f4a460","goldenrod":"daa520","chocolate":"ab5915","brown":"a52a2a","brick":"822222","crimson":"dc143c","platinum":"e5e4e2","electrum":"94826a","bronze":"cd7f32","brass":"b5a642"}
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

return f'-title "{title}" -desc "{txt}" -thumb {thumb} {f"-color {color}" if color is not None else ""} {f"-image {img}" if img else ""}'
</drac2>