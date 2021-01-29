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
#   Always show precise number a spoiler

# approach: cut expression into + terms (*/ later at lower level)
# for each term determine the range and the quadratic expression governing the chance? or the change for each value in the range
# it should recognize number and die size, ro, kh[1] and kl[1] at least (until generalized)
# then iterate, starting with a 0,0 range and empty change array. One by one sum the min and max and multiply the chance
# dict per term in array {term=str, min=int, max=int, p=[probability mass function]}
# first d20: apply adv and remember 1 and 20 as fail and crit. in the end miss% = min(fail,sum) hit=min(crit,sum)

# simple maths: http://www.darkshire.net/~jhkim/rpg/systemdesign/torben_rpg_dice.pdf
# maths: https://stats.stackexchange.com/questions/3614/how-to-easily-determine-the-results-distribution-for-multiple-dice
# dnd maths: https://stats.stackexchange.com/questions/116792/dungeons-dragons-attack-hit-probability-success-percentage/116913#116913
# max dice math: https://math.stackexchange.com/questions/1696623/what-is-the-expected-value-of-the-largest-of-the-three-dice-rolls/
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

# replace first 1d20 with advantage, this is almost the same as !roll, which only does it for the first term
query = query.replace('1d20',['1d20','2d20kh1','3d20kh1','2d20kl1'][args.adv(ea=True)],1)

# append argument bonuses
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
		## Parse number of rolled dice
		dice_exp=term.split('d',maxsplit=1)
		if not dice_exp[0].isnumeric():
			err(f'Dice syntax error, number of dice must be positive number: `{term}`')
		dice=int(dice_exp[0])

		## split in numbers and non numbers
		numeric=None
		dice_part=[]
		for dice_char in dice_exp[1]:
			if numeric is not dice_char.isnumeric():
				dice_part.append('')
				numeric = dice_char.isnumeric()
			dice_part[-1]+=dice_char

		## die size
		if not dice_part or not dice_part[0].isnumeric():
			err(f'Dice syntax error: die size must be a positive number: `{term}`')
		die_size=int(dice_part[0])
		dice_part=dice_part[1:]
		p=[1.0/die_size]*die_size

		## handle operators
		TODO: actually operators are not done in order in the string, but most on the original die and k and d on the set as last

		drop_set={}
		keep_hi=None
		keep_lo=None
		dice_ops=[[dice_part[i],int(dice_part[i+1])] for i in range(0,len(dice_part),2) if dice_part[i+1].isnumeric()]
		while dice_ops:
			op=dice_ops.pop()
			# drop lowest dice is the same as keep highest rest
			if op[0]=='pl':
				keep_lo=dice-op[1]
			elif op[1]=='ph':
				keep_hi=dice-op[1]
			elif op[0]=='kh':
				keep_hi=dop[1]
			elif op[1]=='kl':
				keeo_lo=op[1]
			elif op[0]=='k<':
				TODO drop set all others
			elif op[0]=='k>':
				TODO dropset
			elif op[0]=='k':
				TODO todo dropset
			else op[0]=='p':
				TODO dropset
			# elif op[0].startsWith('ro'):
				# ro1 and and ro<die_size> handle as ro<2 or ro>die_size-1, others ... TBD
				# ro< & ro>, see math
				pass	
			# elif op[0].startsWidth('k'):
				# use minimum of 0, add all dropped chances to p[0]
				pass
			# elif op[0].startsWidth('p'):
				# p: set min of 0, add all dropped values to p[0] set to dropped p[] to 0, k: set rest to 0, with > and < adjust max and slice
				pass	
			# elif op[0]=='mi':
				# mi: sum all chance <mi, increase min and slice 0 off p?
				pass
			# elif op[0]=='ma':
				# ma: sum all chance >ma, decrease max and slice off p
				pass
			else:
				err(f'Dice syntax error: Unsupported operator `{op[0]}` in `{term}`')
				
		# rr, same but infinite is probably: sum all rerolls and normalize the rest

			if keep_hi is not None:
				if keep_lo is not None:
					err('Query error: `{term}` Keeping both high and low dice is not yet supported.)
				# kh: reduce die_size and power 1-p to the power of number of dice-op[1]
				if keep_hi!=1:
					err('Query error: `{term}` Keeping more or less than 1 die is not yet supported.')
				# precomputed binomial coefficient, TODO move to gvar
				bnc = [[], [1], [1, 2], [1, 3, 3], [1, 4, 6, 4], [1, 5, 10, 10, 5], [1, 6, 15, 20, 15, 6], [1, 7, 21, 35, 35, 21, 7], [1, 8, 28, 56, 70, 56, 28, 8], [1, 9, 36, 84, 126, 126, 84, 36, 9]]
				pp=p[:]
				for v in range(die_size):
					np = []
					for d in range(dice):
						np = [sum([npi * pvi for npi in np for pvi in pp[:v]])]
						np += [bnc[dice][-(d + 1)] * pp[v] ** (d + 1)]
					p[v]=sum(np)
				dice=op[1]
			elif keep_lo is not None:
				# kl: reduce die_size and power p to the power of number of dice-op[1]
				dropped_dice=dice-op[1]
				dice-=dropped_dice

		if sign<0:
			p.reverse()
			t={'term':'-'+term,'min':sign*die_size,'max':sign,'p':p}
		else:
			t={'term':term,'min':sign,'max':sign*die_size,'p':p}

		# TODO: This can be optimized here, knowing all pmf are the same. At least for two dice can go through the triangle and double
		# if all chances were even, binomial coefficients could count instead of enumerate the combinations for multiple dice, but comb and factorial aren't available anyway
		terms+=[t]*dice
	else:
		err(f'Dice syntax error: Unrecognized term format: `{term}`')
### combine all terms
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

### Output the report
# TODO: if dbg
report=[]
if dbg:
	report.append(f'{query} : [{lo}] {", ".join(f"{p*100:.2f}%" for p in pmf)}  [{hi}]')

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
		report.append(f'**{t}**: Miss: `{miss*100.0:.1f}%`, Hit: `{(1-miss)*100:.1f}%`')
sep='\n'
return f'echo {sep.join(report)}'

</drac2>
