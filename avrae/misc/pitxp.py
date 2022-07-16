!alias pxp <drac2>
CRxp={str(cr):xp for cr,xp in enumerate([10, 200,450,700,1100,1800,2300,2900,3900,5000,5900,7200,8400,10000,11500,13000,15000,18000,20000,22000,25000,33000,41000,50000,62000,75000,90000,105000,120000,135000,155000])}
CRxp.update({"1/8":25,"1/4":50, "1/2":100})
args=&ARGS&
creatures=[]
for a in args:
	a=a.replace('x','*')
	if '*' in a:
		amt,cr = a.split('*',maxsplit=1)
		if amt.isdigit() and cr in CRxp:
			creatures += [cr]*int(amt)
		else:
			return f'echo Argument error: `{a}`'
	elif a in CRxp:
		creatures.append(a)
	else:
		return f'echo Argument error: `{a}`'
multipliers={0:1, 1:1, 2:1.25, 3:1.5, 7:1.75, 11:2, 15:2.5}
multiplier = [m for q,m in multipliers.items() if q<=len(creatures)][-1]
total_xp=sum(CRxp[cr] for cr in creatures)
return f'echo {total_xp}XP + {(multiplier-1)*100}% = {int(total_xp*multiplier)} '
</drac2>
