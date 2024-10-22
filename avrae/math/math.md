Compute simple equations and functions.

**Examples:**
* `!math 3+2*9` = 3+(2*9) = 21
* `!math (4-2)*3` = 6
* `!math 2^3` = 8 
* `!math sin(pi) + 2` ≈ 2.0
* `!math e^2` = e² ≈ 7.3890
* `!math log(10*10, 10)` = 2.0

**Supported Functions** 
* `sqrt(x)` - square root
* `log(x)` - natural logarithm
* `log(x, y)` - base-`y` logarithm of `x`
* `sin(x)` - sine
* `cos(x)` - cosine
* `tan(x)` - tangent
* `atan(x)` - arctangent
* `atan2(y,x)` - Compute angle of vector.
* `asin(x)` - arcsine
* `acos(x)` - arccosine
* `factorial(n)` - n!
* `comb(n, k)` - Binomial coefficient `n` over `k` is the numbers of ways to choose `k` from `n` items where order doesn't matter.
* `perm(n, k)` - Permutations, ways to choose `k` from `n` items where the order matters.
* `gcd(*args)` - Greatest common divisor of all `args`. 
* `lcm(*args)` - Lowest common multiple of all `args`.
* `rad(degrees)` - Convert degrees to radians
* `deg(radians)` - Convert radians to degrees
* `norm(x[, ...])` - Compute the length of a vector

**Constants**
* `pi`, `e`, `inf`, `π`
* integer and floating point numbers. e.g `1`, `0.1`, `1.0e-1`.