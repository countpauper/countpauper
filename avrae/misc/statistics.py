<drac2>
# Ideas: query attack, spell for base, skill
# * Add all -b
# use adv and replace first 1d20
# Target: combatant (will also roll damage), hp and damage rolls to see if you dying or dead
# rr and say how the chance of how many will hit (and crit?)
# multiple AC/DC,  targets, report chance for each
# - Display result qualitively (certain, impossible, high, medium, low) when targeting characters, (override with ... -s/-h)
# 0 imppssible, <10 very low, <30 low <70 medium <90 high <100 very high, 100 certain
#   option to show as techo (-h <sec>)

# approach: cut expression into + terms (*/ later at lower level)
# for each term determine the range and the quadratic expression governing the chance? or the change for each value in the range
# it should recognize number and die size, ro, kh[1] and kl[1] at least (until generalized)
# then iterate, starting with a 0,0 range and empty change array. One by one sum the min and max and multiply the chance
# dict per term in array {term=str, min=int, max=int, p=[probability mass function]}
# first d20: apply adv and remember 1 and 20 as fail and crit. in the end miss% = min(fail,sum) hit=min(crit,sum)

# simple maths: http://www.darkshire.net/~jhkim/rpg/systemdesign/torben_rpg_dice.pdf
# maths: https://stats.stackexchange.com/questions/3614/how-to-easily-determine-the-results-distribution-for-multiple-dice
# roller (seem to do it the hard waym but good to compare): http://topps.diku.dk/torbenm/troll.msp

#parse query argument
syntax=f'`{ctx.prefix}{ctx.alias} <roll> -ac <number>|-dc <number>`'
args=&ARGS&
if not args:
	return f'echo You did not specify a query. Use: {syntax}'
query=args[0]
args=argparse(args[1:])

dbg=args.last('dbg',True)

# parse targets
cancrit=None
targets=[]
if ac:=args.last('ac') :
	cancrit=True
	if not ac.isdigit():
		return f'echo AC `{ac}` is not a number, Use {syntax}'
	targets=[int(ac)]
elif dc:=args.last('dc') :
	cancrit=False
	if not dc.isdigit():
		return f'echo DC `{dc}` is not a number, Use {syntax}'
	targets=[int(dc)]

# append bonuses
query='+'.join([query]+args.get('b'))

# chance the query < constant
query=query.replace(' ','')
expression=query.replace('+-','-').replace('-','+-').split('+')

terms=[]
crit=None
fail=None
firstd20=True
for term in expression:
	sign=1
	t = None
	if term[0]=='-':
		sign=-1
		term=term[1:]
	if term.isnumeric():
		v=int(term)
		terms+=[{'term':f'{"-" if sign<0 else ""}{term}','min':sign*v,'max':sign*v}]
	elif 'd' in term:
		dicexp=term.split('d',maxsplit=1)
		if not dicexp[0].isnumeric():
			err(f'Dice syntax error, number of dice must be positive number: `{term}`')
		dice=int(dicexp[0])
		# TODO: split operators=['kh','kl','p','rr','ro','ra','mi','ma','e'] or skip all digits of die size
		if not dicexp[1].isnumeric():
			err(f'Dice synax error: die size must be a positive number: `{term}`')
		if dice>0:
			diesize=int(dicexp[1])
		if firstd20 and diesize==20:
			firstd20=False
			# TODO: (dis)advantage = increase diesize and add kh1/kl1 to command str, crit ness
		p=[1.0/diesize]*diesize

		# TODO: ignore dice for now, handle commands
		# kh: reduce diesize and power 1-p
		# kl: reduce diesize and power p
		# mi: sum all chance <mi, increase min and slice 0 off p
		# ma: sum all chance >ma, decrease max and slice off p
		# ro1 and and ro<diesize> handle as ro<2 or ro>diesize-1, others ... TBD
		# ro< & ro>, see math
		# rr, same but infinite is ehm ... math
		# p: set to 0, k: set rest to 0, with > and < adjust max and slice

		# then in the end if diesize still >1 then just ehm ... add multiple copies to terms? 2d6 = 1d6+1d6, but is 3d20kh2 2d20kh1+2d20kh1? pbly not

		if sign<0:
			p.reverse()
			t={'term':'-'+term,'min':sign*diesize,'max':sign,'p':p}
		else:
			t={'term':term,'min':sign,'max':sign*diesize,'p':p}
		# TODO: This can be optimized here, knowing all pmf are the same
		terms+=[t]*dice


lo=sum(t['min'] for t in terms)
hi=sum(t['max'] for t in terms)
pmf=[]
for t in terms:
	if tp:=t.get('p'):
		# combine probability mass functions into a new one
		if not pmf:
			#TODO: why is this case special?
			pmf=tp
		else:
			np=[0]*(len(pmf)+len(tp))
			for v1 in range(len(tp)):
				for v0 in range(len(pmf)):
					np[v0+v1]+=pmf[v0]*tp[v1]
			pmf=np

# TODO: if dbg
report=[]
if dbg:
	report.append(f'[{lo}] {", ".join(f"{p*100:.2f}%" for p in pmf)}  [{hi}]')

if not targets:
	report.append(f'You did not specify any targets. Use: {syntax}')
else:
	for t in targets:
		if t>hi:
			miss=1
		elif t<lo:
			miss=0
		else:
			miss=sum(pmf[i] for i in range(t-lo))
		report.append(f'{t}: Miss: {miss*100.0:.1f}%", Hit: {(1-miss)*100:.1f}%')
sep='\n'
return f'echo {sep.join(report)}'

</drac2>
