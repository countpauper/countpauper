<drac2>
# parse arguments
syntax=f'`{ctx.prefix}{ctx.alias} "<spell>" [<cast arguments>]` or `{ctx.prefix}{ctx.alias} Tree`'
a=&ARGS&
if not a or a[0].lower() in ['?','help']:
	return f'echo Use: {syntax}'
cast_args="""&*&"""

# create the CC
cc='Staff of the Woodlands'
C=character()
C.create_cc_nx(cc, 0, 10, reset='long',reset_by='1d6+4')

tree_form="Tree Form"
# match the spell name
spells={
	"Animal Friendship":1,
	"Awaken":5,
	"Barkskin":2,
	"Locate Animals or Plants":2,
	"Speak with Animals":1,
	"Speak with Plants":3,
	"Wall of Thorns":6,
	"Pass without Trace":0,
	tree_form:1
}

spell=a[0]
match=[sn for sn in spells.keys() if spell.lower() in sn.lower()]
if not match:
	return f'echo Use: {syntax}.\nSupported spells: {", ".join(spells.keys())}.\nCase insensitive and partial name is supported.'
if len(match)>1:
	return f'echo Multiple spells match `{spell}`: {", ".join(match)}.'
match=match[0]
cost=spells[match]

if argparse(a).last('i'):
	fields=f'-f  "{cc}|Not used|inline" '
else:
	# remove the charges
	current=C.get_cc(cc)
	if cost>current:
		return f'echo Not enough charges [{cost}] to cast {match} : {C.cc_str(cc)}'
	remaining = C.mod_cc(cc,-cost)
	fields=f' -f "{cc}[-{cost}]|{cc_str(cc)}|inline" '

	# Did it break?
	if remaining==0:
		r=vroll('1d20')
		if r.total==1:
			fields+=f'-f "Last charge expended|{r}\nThe staff becomes a nonmagical Quarterstaff!|inline" '
		else:
			fields+=f'-f "Last charge expended|{r}\nThe staff retains its magic.|inline" '

#output alias
if match==tree_form:
	desc = """You can use an action to plant one end of the staff in fertile earth and expend 1 charge to transform the staff into a healthy tree. 
	The tree is 60 feet tall and has a 5-foot-diameter trunk, and its branches at the top spread out in a 20-foot radius. The tree appears ordinary but radiates a faint aura of Transmutation magic if targeted by Detect Magic. 
	While touching the tree and using another action to speak its Command, word, you return the staff to its normal form. Any creature in the tree falls when it reverts to a staff."""
	return f'embed -title "{name} plants their {cc}" -desc "{desc}" {fields}'
else:
	return f'cast "{match}" {cast_args} -title "<name> casts [sname] with a {cc}" -i {fields}'
</drac2>
