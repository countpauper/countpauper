# Avrae Math library 

## Introduction

The Avrae math library is originally available in 
`!gvar 5f72a0b9-57c3-4223-a7a4-272e57b9bfc3` and can be imported in an alias with:
```
using(math="5f72a0b9-57c3-4223-a7a4-272e57b9bfc3")
```
This will give access to the `math` namespace containing the functions below.

Most functions will behave similar to the [standard python math library](https://docs.python.org/3/library/math.html), with some differences due to avrae limitations or stubbornness of the developer.

## Contact 

Contact me on the Avrae discord server with @Velglarn or contact me through the github on which this file is hosted with bugs, requests and questions.

## Difference with math
### Accuracy and performance

Many functions are approximated with polynomial expansions, because avrae does not have access to the built in functions and hardware instructions that provide the best performance and accuracy. In fact this avrae limitation is the main motivation for creating this library.

Most functions achieve about 9 digits of accuracy for any input, at the cost of a number of iterations. Since avrae limits alias runtime to a number of instructions, calling these functions in an inner loop may quickly reach the limit.

The computational complexity and therefore the number of instructions used is constant (if high) for most functions. If it's not, it is documented in the function.

### Missing functions 

The following built in avrae functions are omitted on purpose
* `floor(x)`
* `ceil(x)`
* `fabs(x)` - use abs
* `sqrt(x)` - There's a `squareroot` available instead with different behavior for x<0.

There are also many functions [not (yet) implemented](#todo), because they are not as useful 
in the Avrae context. Some of these may be implemented in the future as the need arises. Send me (@Velglarn) a message in the Avrae discord server if you want them added.

### Different function interface

Most functions have the same interface with the following exceptions:
* `prod` does not take an iterable as first argument but computes the product of all arguments. See its [interface](#prod) documentation below for details.
* `pow` also does not take two arguments but will iteratively raise the result to the power of all arguments. e.g. `pow(2,2,2) == 2**2**2`. See its [interface[(#pow)] below for details.

# Function Interface Documentation 

## Constants 
The following constants are defined: `pi`, `tau`, `e`, `inf` and `nan`.

## isnan
`isnan(x): bool` - determine if `x` is [not a number](https://en.wikipedia.org/wiki/NaN). 
Most functions will return NaN if the input is NaN. Some may return NaN also for input that leads to invalid results e.g. 0/0 or inf/inf.

*example:* `math.isnan(math.nan) == True`

## isinf 
`isinf(x): bool` - determine if `x` is infinite. Equivallent to `x == float('inf)`. This will return False for negative infinity. Use `ifinf(-x)` to determine that.

*example:* `math.isinf(math.inf) == True`

## isfinite 
`isfinite(x): bool` - determine if `x` if finite. This will return False for positive *and* negative ifninity.

## isclose 
`isclose(a,b, rel_tol=1e-9, abs_tol=0): bool` - Determine if `a` and `b` are nearly the same. Due to floating point precision and cumulative numerical errors or polynomial approximation some results may not be exactly as expected. 

*example:* `math.isclose(1/3.0, 0.33333, 1e-3) == True`

## squareroot
`squareroot(x): float` - Compute the square root of x, but returns NaN for negative x, because Avrae does not handle exceptions as well.

*example:* `math.squareroot(4.0) == 2.0

## prod
`prod(*values): float` - Compute the product of all arguments. If no arguments are provided the result is 1.0

*example:* `prod(2,3, -1) == -6.0`
 
## pow
`pow(*args): float` - Compute the power of all arguments. If no arguments are provided the result is considered equivalent to 1^1==1.

*example:*`pow(2,2,-1) == 0.25`

## log
`log(x, base=None): float` - Compute the logarithm of x. If not specified the base is `e` to compute the natural logarithm of x. 

*example:*`log(100,10) == 2`

## factorial
`factorial(n): int` - Compute the factorial `n!`. If n is not an integer it will be converted with `floor`. 

The computation complexity of this function is O(n) and beware that for relatively small numbers the result may become huge. Avrae can handle up to n=20.

*example:* `factorial(4) == 24`

## comb
`comb(n, k): int` - Compute the [binomial coefficient](https://en.wikipedia.org/wiki/Binomial_coefficient) n over k. 

The computational complexity of this function is O(n*k).

*example:* `math.comb(4,2) == 6`

## perm
`perm(n, k = None): int` - Compute the number of permutations of k items to choose from n. 

The computational complexity of this function is O(n*k + k)

*example:* `math.perm(4, 2) == 12`

## gcd 
`gcd(*args)` - Compute the greatest common divisor of all arguments. Defaults to 0 for no arguments. 

The computational complexity of this function is O(n log2 v) where v is the smallest number in args and n is the number of arguments.

## lcm 
`lcm(*args)` - Compute the lowest common multiple of all arguments. Defaults to 1 for no arguments.

The computational complexity is O(n + n log2 v).

## degrees
`degrees(rad): float` - Convert radians to degrees

*example:* `math.degrees(math.pi) == 180.0`

## radians
`radians(deg): float` - Convert degrees to radians.

*example:* `math.radians(180) == math.pi`

## sin
`sin(x): float` - Approximate the sine of *x* (rad).

## cos
`cos(x): float` - Approximate the cosine of *x* (rad).

## tan
`tan(x): float` - Approximate the tangent of *x* (rad).

## atan
`atan(x): float` - Compute the arctangent of `x`. 

This approximation converges slowly when abs(x) is closer to 1. For that reason the number of instructions peaks around those values and accuracy may fluctuate over the input range.

## atan2
`atan2(y,x): float` - Compute the angle of the vector (x,y). 
This assumes a right handed coordinate system with the positive x-axis being angle 0.

For all but the trivial inputs this will use `atan` and the same considerations for performance and accuracy applies. 

*example:* `atan2(1,0) == pi/2`

## asin
`asin(x): float` - Compute arcsine of `x`.

This is based on the `atan` implementation so the same performance and accuracy limitations apply, except the most iterations are needed near x = ±π/2.
Additionally the result may be less accurate near 1.0 and -1.0

## acos 
`acos(x): float` - Compute arccosine of `x`.

This is based on the `atan` implementation so the same performance and accuracy limitations apply, except most iterations are needed near x=0
Additionally the result may be less accurate near -1.0.

## hypot
`hypot(*coordinates): float` - Compute the norm or length of the vector.

*example:* `hypot(0,1,1) == sqrt(2) `

## dist
`dist(p, q): float` - Compute the distance between two points defined as equal sized iterables.

*example:* `dist((0,0,1), (0,1,0)) == sqrt(2)`

## modf(x)
`modf(x): (remainder, integer)` - Split a rational number `x` into its remainder and integral part. Both are signed the same as `x`.
  
# Development

## TODO 
The following functions are not implemented yet, either because they are easily emulated if needed or rarely needed in the context of Avrae.  

Roughly in order of priority:

* Power and logarithmic functions: `cbrt`, `exp`, `exp2`, `expm1`, `log1p`, `log2`, `log10` - These can be emulated with the `**` operator or the `log` function, although likely with worse accuracy or performance than a dedicated algorithm.
* Numeric functions: `fma`, `fsum`, `sumprod`,`isqrt`, `remainder`, `fmod` - Easily badly implemented if needed.
* Floating point functions: `copysign`,  `frexp`, `ldexp`, `isqrt`, `nextafter`, `trunc`, `ulp` - If you're messing with floating points at this level you probably did something wrong.
* Special functions: All of them. Maybe be useful for advanced statistics. 
* Hyperbolic functions: All of them. Could be useful for computing magic missile trajectories ;) 

## PRs

If you want to contribute a PR, be aware that the math functions (and calc alias) are unit tested  
and modifications/additions must also be. The unit tests are developed from VSCode with pytest and a virtual environment with 
`requirements.txt` installed.

With the recommended VSCode extensions the tests should be automatically discovered. 
