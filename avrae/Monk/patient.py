embed -title "{{name}} patiently defends!" -thumb <image>
<drac2>
cc='Ki Points'
ch=character()
args="&*&"
ignore=argparse(args).last('i',False)
MonkLevel = int(get('MonkLevel', 0))
field='-f "Patient Defense|You can spend 1 ki point to take the Dodge action as a Bonus Action on Your Turn." '

# Counter
if not ignore:
	if not ch.cc_exists(cc):
		err(f'You don\'t have {cc}, use `!level Monk {MonkLevel}`')
	if not ch.get_cc(cc):
		err(f'You don\'t have any {cc} left. You need to rest first.')

	ch.mod_cc(cc,-1)
	field+=f'-f "{cc}|{ch.cc_str(cc)}"'
else:
	field+=f'-f "{cc}|Not used"'

me = combat().me if combat() else None
if me:
	me.add_effect('Dodging', '', 1,desc=' Any Attack roll made against you has disadvantage if you can see the attacker, and you make Dexterity Saving Throws with advantage. You lose this benefit if you are Incapacitated or if your speed drops to 0.')
return field
</drac2>
