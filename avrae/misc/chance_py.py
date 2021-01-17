def roll(die):
	return [1.0/die]*die

def ro(reroll, p):
	if type(reroll)==int:
		reroll=range(reroll)
	# in draconic  {{ p[min(r):max(r)+1] }}
	reroll_chance=sum(p[reroll.start:reroll.stop:reroll.step])
	base=reroll_chance/len(p)
	p=[base + (0 if v in reroll else ip) for v,ip in enumerate(p)]
	return p

def kh1(dice, p):
	die=len(p)
	pkh1=[]
	for v in range(die):
		pv = [1]
		for d in range(dice):
			np=[]
			# TODO: divide d=2 by d=1  to generalize to any number of dice
			if d==1:
				np+=[dice*ip for ip in p[:v]]
			#for d2 in range(1,d):
			#	np=[ip1*ip2 for ip1 in np for ip2 in p[:v]]
			if d==2:
				np+=[dice*ip0*ip1 for ip0 in p[:v] for ip1 in p[:v]]
			if d==3:
				np+=[dice*ip0*ip1*ip2 for ip0 in p[:v] for ip1 in p[:v] for ip2 in p[:v]]

			pv+=np
			pv=[ip*p[v] for ip in pv]
		pkh1.append(sum(pv))
	return pkh1

if __name__ == "__main__":
	# lamest unit test ever
	print(f"2d4kh1: {kh1(2, roll(4))}  should be 0.0625, 0.1875, 0.3125, 0.4375")
	print(f"3d4kh1: {kh1(3, roll(4))}  {sum(kh1(3, roll(4)))} should be 0.015625, 0.109375, 0.29788, 0.57813" )
	print(f"4d4kh1: {kh1(4, roll(4))}  {sum(kh1(4, roll(4)))} should be 0.0391, 0.05859, 0.25391, 0.68359" )
	print(f"1d20roX: {ro(1, roll(20))}  {sum(ro(1, roll(20)))} should be 0.0025, 0.0525...." )
	print(f"1d4kh1: {kh1(1, roll(4))}  should be all 0.25")

