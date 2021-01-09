<drac2>
cc='Ki Points'
ch=character()
args=&ARGS&

if ignore:=argparse(args).last('i',False):
	args.remove('-i')

if not args:
	return f'echo Use: `{ctx.prefix}{ctx.alias} arts <spell> [-i] <cast arguments>.'

MonkLevel = int(get('MonkLevel', 0))
field='-f "Shadow Arts|You can use your ki to duplicate the effects of certain spells." '

if not ignore:
	if not ch.cc_exists(cc):
		err(f'You don\'t have {cc}, use `!level Monk {MonkLevel}`')
	if ch.get_cc(cc)<2:
		err(f'You don\'t have anough {cc} left. You need to rest first.')

	ch.mod_cc(cc,-1)
	field+=f'-f "{cc}|{ch.cc_str(cc)}"'
else:
	field+=f'-f "{cc}|Not used"'
return f'cast {" ".join(args)} -i {field}'
</drac2>
