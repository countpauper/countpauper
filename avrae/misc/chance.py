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
			err(f'Dice expression syntax error: number of dice must be positive number: `{term}`')
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
			err(f'Dice expression syntax error: die size must be a positive number: `{term}`')
		die_size=int(dice_part[0])
		dice_part=dice_part[1:]
		p=[0]+[1.0/die_size]*die_size # 0 is drop chance, p[v] for rolling v

		## handle operators
		drop_set={}
		keep_hi=None
		keep_lo=None
		drop_chance=0
		dice_ops=[[dice_part[i],int(dice_part[i+1])] for i in range(0,len(dice_part),2) if dice_part[i+1].isnumeric()]
		while dice_ops:
			op=dice_ops.pop(0)
			arg_v=op[1]
			operator=op[0].lower()

			# drop lowest dice is the same as keep highest rest
			if operator=='pl':
				keep_hi=dice-arg_v
			elif operator=='ph':
				keep_lo=dice-arg_v
			elif operator=='kh':
				keep_hi=arg_v
			elif operator=='kl':
				keep_lo=arg_v
			else:
				# not dropping hi or lo, handle the other operators which do not reduce the number of dice
				arg_v=min(max(0,arg_v),die_size)
				if operator[-1]=='<':
					operator=operator[:-1]
					selection=range(1,arg_v)
				elif operator[-1]=='>':
					operator=operator[:-1]
					selection=range(arg_v+1,die_size+1)
				elif arg_v>0 and arg_v<=die_size:
					selection=[arg_v]
				else:
					selection=[]
				if operator=='k':
					# TODO: k1k2k3 doesn't work because it drops everything
					p[0]+=sum(pv for v,pv in enumerate(p) if v not in selection and v>0)
					p=[pv if (v==0 or v in selection) else 0 for v,pv in enumerate(p)]
				elif operator=='p':
					p[0]+=sum(pv for v,pv in enumerate(p) if v in selection)
					p=[pv if (v==0 or v not in selection) else 0 for v,pv in enumerate(p)]
				elif operator=='ro':
					reroll_chance = sum(pv for v,pv in enumerate(p) if v in selection)
					p = [(reroll_chance/die_size if v>0 else 0) + (0 if v in selection else pv) for v, pv in enumerate(p)]
				elif operator=='rr':
					reroll_chance = sum(pv for v,pv in enumerate(p) if v in selection)
					rerolled_chance = reroll_chance / (die_size - len(selection))
					p = [pv if v==0 else 0 if v in selection else pv + rerolled_chance for v, pv in enumerate(p)]

#					inv_reroll_chance = 1-sum(pv for v,pv in enumerate(p) if v in selection)
#					p = [pv if v==0 else 0 if v in selection else pv/inv_reroll_chance for v, pv in enumerate(p)] # to FIX 1d4p1rr<3  should be roughly 25%, 0.0%, 0.0%, 37.5%, 37.5%
				elif operator=='mi':
					selection=range(1,arg_v)
					missed_chances=sum(pv for v,pv in enumerate(p) if v in selection)
					p=[0 if v in selection else pv for v,pv in enumerate(p)]
					p[arg_v]+=missed_chances
				elif operator=='ma':
					selection=range(arg_v+1,len(p))
					missed_chances=sum(pv for v,pv in enumerate(p) if v in selection)
					p=[0 if v in selection else pv for v,pv in enumerate(p)]
					p[arg_v]+=missed_chances
				elif operator=='e':
					err(f'Implementation limitation: Exploding dice not supported in `{term}`')
				else:
					err(f'Dice expression syntax error: Unrecognized operator `{operator}` in `{term}`')

		# precomputed binomial coefficient, TODO move to gvar
		bnc = [[], [1], [1, 2], [1, 3, 3], [1, 4, 6, 4], [1, 5, 10, 10, 5], [1, 6, 15, 20, 15, 6],
			   [1, 7, 21, 35, 35, 21, 7], [1, 8, 28, 56, 70, 56, 28, 8], [1, 9, 36, 84, 126, 126, 84, 36, 9]]

		if keep_hi is not None and keep_hi<dice:
			if keep_hi!=1:
				err(f'Implementation limitation: `{term}` Keeping more or less than 1 die is not yet supported.')	 # also covers keep_hi>1 or keep_lo>1
			pp=p[:]
			for v in range(len(p)):
				np = []
				for d in range(dice):
					np = [sum([npi * pvi for npi in np for pvi in pp[:v]])]
					np += [bnc[dice][-(d + 1)] * pp[v] ** (d + 1)]
				p[v]=sum(np)
			dice=keep_hi
		if keep_lo is not None and keep_lo<dice:
			if keep_lo!=1:
				err(f'Implementation limitation: `{term}` Keeping more or less than 1 die is not yet supported.')
			pp=p[:]
			for v in range(len(p)):
				np = []
				for d in range(dice):
					np = [sum([npi * pvi for npi in np for pvi in pp[:v]])]
					np += [bnc[dice][-(d + 1)] * pp[v] ** (d + 1)]
				p[v]=sum(np)
			dice=keep_lo

		if sign<0:
			p.reverse()
			t={'term':'-'+term,'min':sign*die_size,'max':0,'p':p}
		else:
			t={'term':term,'min':0,'max':sign*die_size,'p':p}

		# TODO: This can be optimized here, knowing all pmf are the same. At least for two dice can go through the triangle and double
		# if all chances were even, binomial coefficients could count instead of enumerate the combinations for multiple dice, but comb and factorial aren't available anyway
		terms+=[t]*dice
	else:
		err(f'Dice expression syntax error: Unrecognized term format: `{term}`')
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
