embed -title "{{name}} steps with the wind!" -thumb <image>
<drac2>
cc='Ki Points'
ch=character()
args="&*&"
ignore=argparse(args).last('i',False)
MonkLevel = int(get('MonkLevel', 0))
field='-f "Step of the Wind|You can spend 1 ki point to take the Disengage or Dash action as a Bonus Action on Your Turn, and your jump distance is doubled for the turn." '
if not ignore:
	if not ch.cc_exists(cc):
		err(f'You don\'t have {cc}, use `!level Monk {MonkLevel}`')
	if not ch.get_cc(cc):
		err(f'You don\'t have any {cc} left. You need to rest first.')

	ch.mod_cc(cc,-1)
	field+=f'-f "{cc}|{ch.cc_str(cc)}"'
else:
	field+=f'-f "{cc}|Not used"'
return field
</drac2>
