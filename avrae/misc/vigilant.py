FIX: reuse when used
title

!alias vigilant <drac2>
arg="&*&"
if not arg:
	arg=name

c=character()
ccn='Vigilant Blessing'
c.create_cc_nx(ccn,'0','1','long','bubble')
cc=c.cc(ccn)
if not cc.value:
	return f'echo {ccn} if not ready until next long rest: {cc}'
cc.set(0)
return f"""embed -title "{arg} is Blessed by Vigilance"
-desc "The night has taught you to be vigilant. As an action, you give one creature you touch (including possibly yourself) advantage on the next initiative roll the creature makes. This benefit ends immediately after the roll or if you use this feature again."
-f "{arg.title()}|Next combat use `!i join adv`|inline" 
-f "{ccn}|{cc}|inline" """
</drac2>