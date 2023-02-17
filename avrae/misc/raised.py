!servalias raised <drac2>
ch=character()
cc='Resurrection sickness'
desc='''Coming back from the dead is an ordeal. The target takes a −4 penalty to all attack rolls, saving throws, and ability checks. Every time the target finishes a long rest, the penalty is reduced by 1 until it disappears.'''
if not ch.cc_exists(cc):
	ch.create_cc(cc,minVal=0, maxVal=4, reset='long',dispType='square',reset_by=-1, initial_value=4, desc=desc)
penalty=ch.get_cc(cc)
field=f'-f "{cc}|{":skull:"*ch.get_cc(cc)}|inline"'
if penalty==0:
	ch.delete_cc(cc)
	return f'embed -title "{name} is cured from {cc}" {field} -footer "Use {ctx.prefix}{ctx.alias} again after being resurrected or raised."'

c=combat()
if not c or not c.me:
	return f'''embed -title "{name} has {cc}" -desc "{desc}" {field} -footer "Use {ctx.prefix}{ctx.alias} after joining combat."'''
c.me.add_effect(cc, desc=desc, passive_effects=dict(to_hit_bonus=-penalty,save_bonus=-penalty,check_bonus=-penalty))
return f'embed -title "{name} has {cc}" -desc "{desc}" {field} -f "Penalty|{penalty}|inline"'
</drac2>