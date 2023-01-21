import math
import numpy as np

def solve(*terms):
	a,b,c = terms
	"Solve polynomial ax^2 +bx + c =0. Will return a tuple of solutions"
	if a==0:	# linear bx+c =0
		if b==0:	# no solution, c=0
			return tuple()
		else:
			return -c/b,
	else:
		d=b*b - 4*a*c
		if d == 0:
			return -b / (2*a),
		elif d < 0:
			return tuple()
		else:
			return ((-b - math.sqrt(d)) / (2*a),
					(-b + math.sqrt(d)) / (2*a))


def compute_poly(x, *terms):
	return sum(term * x**power for power,term in enumerate(reversed(terms)))

# TODO: arbitrary order (well 1st, 2nd 3rd at least) by number of arguments. If there are 0s then automatically reduce
# TODO: solve cubic https://en.wikipedia.org/wiki/Cubic_equation#Discriminant_and_nature_of_the_roots
# TODO: solve differential equations by solving the characteristic polynomial with this (characteristic polynomial has same term constants)
#  AND/OR [done] solve differential equations using the matrix' eigenvalues (should be the same)
# TODO unit tests
print(compute_poly(3, 2, 1, 4))
print(solve(0, 2, -4))
print(solve(1, 0, 0))
print(solve(1, 0, 2))
print(solve(1, 0, -1))
print(solve(2, 4, -4))

# https://www.youtube.com/watch?v=fi54Hz5TiWI&list=PLMrJAkhIeNNTYaOnVI3QpH7jgULnAmvPA&index=10
def solve_lde(A):
	eVals, eVecs = np.linalg.eig(A)
	return eVals, eVecs

def solve_lde_with_eigen(*terms):
	a,b,c= terms  # ax" + bx' +cx = 0
	# x" = -(b/a)x' - (c/a)x
	# A=  # [v',v"] = A * [v, v']
	return solve_lde(np.array([[0, 1], [-c/a, -b/a]]))

print(f"eigen: { solve_lde_with_eigen(1,-3,2)}")
print(f"eigen: { solve_lde_with_eigen(1, 1,-2)}")


def diagonalize(A):
	eVals, eVecs = np.linalg.eig(A)
	return np.diag(eVals), eVecs
A=np.array([[8, 10, 10], [0, 3, 0], [-5, -10, -7]])
D, S = diagonalize(A)
Sinv=np.linalg.inv(S)

print(f'Diagonal({A})) = { diagonalize(A)}')
print(S.dot(D.dot(Sinv)))
