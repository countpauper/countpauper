<drac2>
# Ideas: query attack, spell
# Target: combatant (will also roll damage), hp and damage rolls to see if you dying or dead
# rr and say how the chance of how many will hit (and crit?)
# multiple AC/DC targets, report chance for each

#parse query

syntax=f'`{ctx.prefix}{ctx.alias} <roll> -ac <number>|-dc <number>`'

args=&ARGS&
if not args:
	return f'echo You did not specify a query. Use: {syntax}'
query=args[0]
args=argparse(args[1:])

# parse targets
crit=None
targets=[]
if ac:=args.last('ac') :
	crit=True
	if not ac.isdigit():
		return f'echo AC `{ac}` is not a number, Use {syntax}'
	targets=[int(ac)]
elif dc:=args.last('dc') :
	crit=False
	if not dc.isdigit():
		return f'echo DC `{dc}` is not a number, Use {syntax}'
	targets=[int(dc)]

if not targets:
	return f'echo You did not specify any targets. Use: {syntax}'

#TODO: split */
r={'':1.0}
query=query.replace(' ','')
expression=query.replace('+-','-').replace('-','+-').split('+')
# TODO: remove [stuff]
for exp in expression:
	if exp[0]=='-':
		sign=-1
		exp=exp[1:]
	else:
		sign=1
	if exp.isdigit():
		# modify all values
		delta=sign*int(exp)
		r={'+'.join([str((int(v) if v else 0)+delta) for v in values.split('+')]):chance for values,chance in r.items()}
	elif 'd' in exp:
		dicexp=exp.split('d',maxsplit=1)
		if not dicexp[0].isdigit():
			err(f'Dice syntax error, number of dice must be positive number: `{exp}`')
		dice=int(dicexp[0])
		if dice>10:
			err(f'Dice syntax error, for performance reasons the number of dice should be be at most 10: `{exp}`')
		# TODO: split operators=['k','p','rr','ro','ra','mi','ma','e']
		if not dicexp[1].isdigit():
			err(f'Dice synax error: die size must be a positive number: `{exp}`')
		if dice>0:
			diesize=int(dicexp[1])
			if diesize>100:
				err(f'Dice syntax error, for performance reasons the die size should be at most 100: `{exp}`')
			p=1.0/diesize
			newrolls={str(v):p for v in range(1,diesize+1)}
			for die in range(1,dice):
				newrolls={values+'+'+str(v):chance*p for v in range(1,diesize+1) for values,chance in newrolls.items() }

			# TODO operators on newdice
			# sum and flatten
			r={(values+'+' if values else '')+newvalues:chance*newchance for values,chance in r.items() for newvalues,newchance in newrolls.items()}
	else:
		err(f'Expression syntax error, must be a dice or number: {exp}')

# sum
result={}
for values,chance in r.items():
	value = sum([int(v) for v in values.split('+') if v])
	result[value]=result.get(value,0)+chance

#return f'echo {result}'
report=[]
for t in targets:
	hit=sum([chance for total,chance in result.items() if total>=t])*100.0
	miss=sum([chance for total,chance in result.items() if total<t])*100.0
	report.append(f'Target {t}: Hit: {round(hit)}%, Miss: {round(miss)}%')
return f'echo {query} : {", ".join(report)}'

</drac2>