embed <drac2>
args=&ARGS&
amount = min(3,int(args[0])) if len(args)>=1 and args[0].isdecimal() else 1
base = int(args[1]) if len(args)>=2 and args[1].isdecimal() else None
ch=character()
cc='Ki Points'
if not ch.cc_exists(cc):
	err(f'You don\'t have {cc}, use `!level Monk`')
left=ch.get_cc(cc)
if left<amount:
	err(f'You have {left}/{amount} {cc} left. You need to rest first.')
ch.mod_cc(cc, -amount)
field = f'-f "{cc}|{ch.cc_str(cc)}  [-{amount}]|inline"'
if base is None:
	field+=f' -f "To hit|+{2*amount}|inline"'
else:
	r=vroll(f'{base}+2*{amount}')
	field+=f' -f "To hit|{r}|inline"'
return f'{field} -title "{name} focuses {get("their","their")} aim" '
</drac2> --thumb <image> -color <color> -desc "When you miss with an attack roll, you can spend 1 to 3 ki points to increase your attack roll by 2 for each of these ki points you spend, potentially turning the miss into a hit."
