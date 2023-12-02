from math import sqrt
import numpy as np

class Function:
	pass


# TODO:
#  Can use numpy matrices for coefficients and operate on vectors as well as scalars?
#  Least squares fitting with a polynomial output
#  Solve cubic polynomials
#  Other types of functions like Reciprocal(Function) and Exponent(Function) ?
# NB: Polynomial is an endo-functor
class Polynomial(Function):
	def __init__(self, *args):
		if len(args) == 1 and hasattr(args[0], '__iter__'):
			self.coefficients = list(args[0])
		else:
			self.coefficients = list(args)
			self.coefficients.reverse()
		self._optimize()

	def _optimize(self):
		for idx,v in enumerate(reversed(self.coefficients)):
			if v!=0:
				if idx>0:
					self.coefficients = self.coefficients[:-idx]
				break
		else:
			self.coefficients=[]

	def __call__(self, v):
		terms = [(v**order)*c for order, c in enumerate(self.coefficients)]
		if isinstance(v, (int, float, complex)):
			return sum(terms)
		else: # Non numeric sum (for adding Functions)
			if not terms:
				return type(v)()
			o=terms[0]
			for t in terms[1:]:
				o+=t
			return o

	def __add__(self, v):
		# adding a scalar is this even a thing? T
		if type(v) == int or type(v) == float:
			return Polynomial(*([self.coefficients[0]+v]+self.coefficients[1:]))
		if type(v) == Polynomial:
			c1 = self.coefficients
			c2 = v.coefficients
			if len(c1)<len(c2):
				c1+=[0]
			elif len(c1)>len(c2):
				c2+=[0]*(len(c1)-len(c2))
			return Polynomial([a+b for a,b in zip(c1,c2)])
		raise TypeError(type(v).__name__ + "is an invalid type to add to a polynomial")

	def __sub__(self, v):
		return self.__add__(-v)

	def __mul__(self, v):
		if type(v)==float or type(v)==int:
			return Polynomial([c * v for c in self.coefficients])
		if type(v) == Polynomial:
			return Polynomial(np.convolve(self.coefficients, v.coefficients))
		raise TypeError(type(v).__name__ + "is an invalid type to multiply a polynomial with")

	def __pow__(self, n):
		#if type(n)!=int:
		#	return self.__pow__(int(n))
		if type(n)!=int and not n.is_integer():
			raise ValueError("Root of polynomial functions are not (yet) supported")
		if n == 0:
			return Polynomial(1)
		elif n == 1:
			return self
		elif n == 2:
			return self * self
		elif n<0:
			# could be Reciprocal(self**-n) with Reciprocal a separate function,
			#   but then why not Division(function,function) as a function or defined as a Multiple of Reciprocal
			raise ValueError("Reciprocal of a polynomial is not (yet) supported")
		else:
			return (self**n-1) * self

	def __neg__(self):
		return Polynomial([-c for c in self.coefficients])

	def __len__(self):
		return len(self.coefficients)

	def __eq__(self, other):
		return self.coefficients == other.coefficients

	def _formatTerm(c, order):
		if c == 0:
			return None
		if order == 0:
			if c == 0:
				return None
			else:
				return f'{c}'
		return ('' if c==1 else str(c)) + 'x' + ('' if order==1 else f'^{order}')

	def __str__(self):
		terms = [Polynomial._formatTerm(c, order) for order, c in enumerate(self.coefficients)]
		terms.reverse()
		return '+'.join(t for t in terms if t is not None).replace('+-','-')

	def derivative(self):
		return Polynomial((order+1)*c for order, c in enumerate(self.coefficients[1:]))

	def integral(self):
		return Polynomial(['c']+[c/(order+1) for order, c in enumerate(self.coefficients)])

	def solve(self, value=0):
		if len(self)==0:	# can not solve a null function
			return None
		elif len(self)==1:
			if value==self.coefficients[0]:
				return None	# actually all numbers
			else:
				return {}
		elif len(self)==2:
			return {self._solve_linear(value)}
		elif len(self)==3:
			return self._solve_quadratic(value)
		#elif len(self)==4:
		#	return self._solve_cubic(value)
		else:
			raise RuntimeError("Solving a "+len(self)+"th order Polynomial is not yet implemented")

	def _solve_linear(self, v):
		#ax+b=v ax=v-b x = (v-b)/a
		b, a = self.coefficients
		return (v-b)/a

	def _solve_quadratic(self,v):
		# ax^2+bx+c = v => ax^2+bx+c-v = 0
		c, b, a=self.coefficients
		c-=v
		D=b**2 -4*a*c
		vertex = -b/2*a
		if D < 0:
			return {complex(vertex, sqrt(-D)/2*a), complex(vertex, -sqrt(-D)/2*a)}
		elif D == 0:
			return {vertex}
		else:
			return {(-b+sqrt(D))/2*a, (-b-sqrt(D))/2*a}

	def _solve_cubic(self, v):
		d, c, b, a = self.coefficients
		d-=v
		delta_0=b**2 -3*a*c
		delta_0=2*b**3 - 9*a*b*c + 27*a**2 * d

assert(Polynomial()(10)==0)
assert(Polynomial(1)(0)==1)
assert(Polynomial(0,1)==Polynomial(1))
assert(Polynomial(1, 2)(2)==4)
assert(str(Polynomial(1, 0, -2,3.1))=="x^3-2x+3.1")
assert(str(Polynomial('a', -2, 'c'))=="ax^2-2x+c")
assert(Polynomial(1)+3 == Polynomial(4))
assert(Polynomial(1) + Polynomial(1,3) == Polynomial(1,4))
assert(Polynomial(1)-3 == Polynomial(-2))
assert(Polynomial(1, 3) - Polynomial(1) == Polynomial(1,2))
assert(Polynomial(2, 1)*-2.0 == Polynomial(-4, -2))
assert(Polynomial(2, 1)**2.0 == Polynomial(4, 4, 1))
assert(Polynomial(1, 2, 3)*Polynomial(4, 5) == Polynomial(4, 13, 22, 15))
assert(Polynomial(2, 1)(Polynomial(3,2)) == Polynomial(6,5))
assert(Polynomial(5).solve(4) == {})
assert(Polynomial(3, -2).solve(7) == {3})
assert(Polynomial(1, 0, 0).solve() == {0})
assert(Polynomial(1, -6, 0).solve(16) == {-2, 8})
assert(Polynomial(1, -4, 5).solve(0) == {complex(2, -1), complex(2,1)})

assert(Polynomial(2,3).derivative() == Polynomial(2))
assert(Polynomial(2,-1, 5).derivative() == Polynomial(4, -1))
assert(Polynomial(3,-1, 5).integral() == Polynomial(1, -0.5, 5, 'c'))
assert(Polynomial(-3, 2, -0.5).integral().derivative() == Polynomial(-3, 2, -0.5))
