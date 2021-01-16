

def roll(die):
	return [1.0/die]*die


def kh1(dice, p):
	op=p[:]
	die=len(p)
	for v in range(die):
		# This works for 2dXkh1 but not for 3dx
		#p[v]=op[v]**dice + dice * op[v]*sum(op[i]**(dice-1) for i in range(v))

		# this is a very specific one for 3 dice
		# corner
		pv = 1
		# edges
		for d in range(dice):
			#if d==0:
			#	pv+= op[v] ** (dice - 1)

			# TODO: divide d=2 by d=1  to generalize to any number of dice
			if d==1:
				for v0 in range(0,v):
					pv+=op[v0] * dice

			if d==2:
				for v0 in range(0,v):
					for v1 in range(0,v):
						pv += op[v0]*op[v1]*dice
			pv*=op[v]
		p[v]=pv
	return p

if __name__ == "__main__":
	print(f"1d4kh1: {kh1(1, roll(4))}")
	print(f"2d4kh1: {kh1(2, roll(4))}")
	print(f"3d4kh1: {kh1(3, roll(4))}")

