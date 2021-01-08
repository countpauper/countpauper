<drac2>
cc='Ki Points'
ch=character()
args="&*&"
ignore=argparse(args).last('i',False)
MonkLevel = int(get('MonkLevel', 0))
field='-f "Flurry of Blows|Immediately after you take the Attack action on Your Turn, you can spend 1 ki point to make two unarmed strikes as a Bonus Action." '
if not ignore:
	if not ch.cc_exists(cc):
		err(f'You don\'t have {cc}, use `!level Monk {MonkLevel}`')
	if not ch.get_cc(cc):
		err(f'You don\'t have any {cc} left. you need to rest first.')

	ch.mod_cc(cc,-1)
	field+=f'-f "{cc}|{ch.cc_str(cc)}"'
else:
	field+=f'-f "{cc}|Not used"'

targets=argparse(args).get('t')
if len(targets)==1:
	rr='-rr 2'
else:
	rr =''
# TODO should count attacks out of 2 and add a field inline with cc, perhaps a footer with the use

return f'attack Unarmed {args} {rr} {field} -title "[name] attacks with a Flurry of Blows!"'
</drac2>

