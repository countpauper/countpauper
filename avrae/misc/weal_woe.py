!alias weal <drac2>
cc="Cosmic Omen"
c=character()
if not c.cc_exists(cc):
	return f'echo No {cc} counter. Use `!level`'
if c.get_cc(cc)<=0:
	return f'echo Not enough {cc}s. You need to rest.'
c.mod_cc(cc,-1)
r=vroll('1d6')
if 'weal' in ctx.alias:
	b=r.total
	add_sub=f'add the number rolled ({r}) to'
else:
	b=-r.total
	add_sub=f'subtract the number ({r}) rolled from'
target='''&*&'''.title() or "Target"
return f'''embed -title "The Cosmic Omen comes true!" 
-desc "**{ctx.alias.title()}.** Whenever a creature you can see within 30 feet of you is about to make an attack roll, a saving throw, or an ability check, you can use your reaction to roll a d6 and {add_sub} tfrom the total."
-f "{target}, Use:|`-b {b}`|inline" -f "{cc}|{c.cc_str(cc)}|inline" '''
</drac2>
