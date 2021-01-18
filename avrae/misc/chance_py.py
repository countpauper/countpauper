# reference: https://topps.diku.dk/torbenm/troll.msp

def roll(die):
	return [1.0/die]*die

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
	pass
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
		comb=[[],[1],[1,2],[1,3,3],[1,4,6,4],[1,2,7,10,5]]
		pv = [comb[dice][0]]
		for d in range(dice):
			np=[]
			# TODO: divide d=2 by d=1  to generalize to any number of dice
			if d==1:
				np+=[comb[dice][d]*ip for ip in p[:v]]
			#for d2 in range(1,d):
			#	np=[ip1*ip2 for ip1 in np for ip2 in p[:v]]
			if d==2:
				np+=[comb[dice][d]*ip0*ip1 for ip0 in p[:v] for ip1 in p[:v]]
			if d==3:
				np+=[comb[dice][d]*ip0*ip1*ip2 for ip0 in p[:v] for ip1 in p[:v] for ip2 in p[:v]]

			pv+=np
			pv=[ip*p[v] for ip in pv]
		pkh1.append(sum(pv))
	return pkh1

if __name__ == "__main__":
	# lamest unit test ever
	print(f"2d4kh1: {kh1(2, roll(4))}  should be 0.0625, 0.1875, 0.3125, 0.4375")
	print(f"3d4kh1: {kh1(3, roll(4))}  {sum(kh1(3, roll(4)))} should be 0.015625, 0.109375, 0.29788, 0.57813" )
	print(f"4d4kh1: {kh1(4, roll(4))}  {sum(kh1(4, roll(4)))} should be 0.0391, 0.05859, 0.25391, 0.68359" )
	print(f"1d20roX: {ro(10, roll(20))}  {sum(ro(10, roll(20)))} should be 0.0025, 0.0525...." )
	print(f"1d4kh1: {kh1(1, roll(4))}  should be all 0.25")

