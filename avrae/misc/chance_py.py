import math
# reference: https://topps.diku.dk/torbenm/troll.msp

def bnc(n,k):
	return math.factorial(n)/(math.factorial(k)*math.factorial(n-k))


def precompute(dice):
	return [[int(bnc(die,d)) for d in range(die)] for die in range(0,dice)]

def roll(die):
	return [1.0/die]*die

def rolls(dice, p):
	op=p
	for _ in range(1,dice):
		np=[0]*(len(op)+len(p)-1)
		for v0,p0 in enumerate(p):
			for v1,p1 in enumerate(op):
				np[v0+v1]+=p0*p1
		op=np
	return op

# Troll: repeat x:=1d20 while x<=5
def rr(reroll, p):
	if type(reroll)==int:
		reroll=range(reroll)
	# in draconic  {{ p[min(r):max(r)+1] }}
	chance=1-sum(p[reroll.start:reroll.stop:reroll.step])
	p=[(0 if v in reroll else ip/chance) for v,ip in enumerate(p)]
	return p

#\ Halfling luck 1d20ro1
#x:=1d20;
#if x=1 then 1d20 else x
def ro(reroll, p):
	if type(reroll)==int:
		reroll=range(reroll)
	# in draconic  {{ p[min(r):max(r)+1] }}
	reroll_chance=sum(p[reroll.start:reroll.stop:reroll.step])
	base=reroll_chance/len(p)
	p=[base + (0 if v in reroll else ip) for v,ip in enumerate(p)]
	return p


def kh(dice, keep, p):
	kh1p=kh1(p)
	sumkh1p=[sum(kh1p[:v]) for v in range(len(kh1p))]

	for _ in range(1,dice):
		np=[0]*(len(op)+len(p)-1)
		for v0,p0 in enumerate(p):
			for v1,p1 in enumerate(op):
				np[v0+v1]+=p0*p1
		op=np
	return op



	rolld=rolls(keep,p)

	totalp=rolls(keep,p)

	return kh1(dice,p)
	# other approach: do hk1(p) with dice =1+dice-keep, this is the chance that the dropped dice are < than V
	# Then enumerate the remaining dice in keep from [1*keep to len(p)*keep]
	# and make combinations [1,2,3] (preferably unique with a multiplier binomial coefficient for performance)
	# 	   unique is a=[0..max] b = [a..max] c=[c..max]
	# then for all combinations add chances for p[sum(combination)] = p([combinations]) * kh1_result[min(combination)] (max for kl)
	# 		so that is the chance to roll that combination times the chance that all other dice are less


	# TODO: enumerate all possible cobminations of two highest kept dice: ie 1+1 1+2 2+1 2+2 etc
	# Compute the chances to have those exact combinations of values as a base for each addition
	# Compute the chances that all other dice are less than or equal to the lowest
	#	This is the same as sum(kh1(dice,p)[:lowest] (probably)
	# multiply these chances
	# sum all chances that have the same end result (ie 1+3, 3+1 and 2+2)

# troll: max 3d20
# TODO

def kh1(dice, p):
	die=len(p)
	pkh1=[]
	for v in range(die):
		# comb(dice,die) where d=dice-d-1
		cb=precompute(dice+1) #[[],[1],[1,2],[1,3,3],[1,4,6,4],[1,5,10,10,5]]
		np = []
		for d in range(dice):
			#d=id-1
			# TODO: divide d=2 by d=1  to generalize to any number of dice
			#if d==3:
			#	np+=[comb[dice][d]*ip0*ip1*ip2 * p[v]**(dice-d) for ip0 in p[:v] for ip1 in p[:v] for ip2 in p[:v]]
			#if d==2:
			#	np+=[comb[dice][d]*ip0*ip1 * p[v]**(dice-d) for ip0 in p[:v] for ip1 in p[:v]]
			#if d==1:
			#	np+=[comb[dice][d]*ip * p[v]**(dice-d) for ip in p[:v]]
			#if d==0:
			#	np+=[comb[dice][d] * p[v]**(dice-d)]

			# print(f'comb({dice},{d} = {bnc(dice,dice-d-1)} vs {cb[dice][d]}')
			# sum to optimize
			np=[sum([npi * pvi for npi in np for pvi in p[:v]])]
			np+=[cb[dice][-(d+1)] * p[v]**(d+1)]
		pkh1.append(sum(np))
	return pkh1

def kl1(dice, p):
	die=len(p)
	pkl1=[]
	for v in range(die):
		cb=precompute(dice+1) #[[],[1],[1,2],[1,3,3],[1,4,6,4],[1,5,10,10,5]]
		np = []
		for d in range(dice):
			np=[sum([npi * pvi for npi in np for pvi in p[v+1:]])]	# p[v+1:] is the whole difference
			np+=[cb[dice][-(d+1)] * p[v]**(d+1)]
		pkl1.append(sum(np))
	return pkl1

def test(label, result, expected, error=0.001):
	errors=[abs(r-e) for r,e in zip(result,expected)]
	fail=any([e>error for e in errors])
	total=sum(result)
	if abs(total-1)>error:
		fail=True
	if fail:
		print(f'{label} FAIL {result} = {total:.3f} expected ['+ ','.join([f'{expect} ({e})' for expect,e in zip(expected,errors)])+']')
	else:
		print(f'{label} SUCCESS {result}')

if __name__ == "__main__":
	print(precompute(10))
	# troll: max 3d20
	test('1d4kh1', kh1(1, roll(4)),  [0.25]*4)
	test('2d4kh1', kh1(2, roll(4)), [0.0625, 0.1875, 0.3125, 0.4375])
	test('3d4kh1', kh1(3, roll(4)),  [0.015625, 0.109375, 0.296875, 0.578125])
	test('4d4kh1', kh1(4, roll(4)) , [0.00390625, 0.05859375, 0.25390625, 0.68359375])
	test('5d4kh1', kh1(5, roll(4)) , [0.000976563, 0.030273438, 0.206054688, 0.762695313])

	test('1d20roX', ro(1, roll(20)),  [0.0025] + [0.0525]*19)
	test('3d4kl1', kl1(3, roll(4)),  [0.57812, 0.29687	, 0.10938, 0.01563])

	test('3d6', rolls(3,roll(6)), [0.004630,0.013889,0.027778,0.046296,0.069444,0.097222,0.115741,0.125000,0.125000,0.115741,0.097222,0.069444,0.046296,0.027778,0.013889,0.004630])
	# x:= 1d4;
	# y:= if x=1 then 1d4 else x;
	# u:=1d4;
	# v:=if u=1 then 1d4 else u;
	# max { y,v }
	test('2d4ro1kh1', kh1(2, ro(1,roll(4))),  [0.00391, 0.13672, 0.33203, 0.5273])

	# TODO: 4d6kh3/dl1 Troll: sum largest 3 4d6
	test('4d6kh3', kh(4, 3, roll(6)),  [0.004630,0.013889, 0.027778, 0.046296, 0.069444, 0.097222, 0.115741, 0.125000, 0.125000, 0.115741, 0.097222,0.069444, 0.046296, 0.027778, 0.013889, 0.004630]) # NB [0] = 3



