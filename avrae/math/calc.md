Compute simple equations and functions.

**Examples:**
* `!calc 3+2*9` = 3+(2*9) = 21
* `!calc (4-2)*3` = 6
* `!calc 2^3` = 8 
* `!calc sin(pi) + 2` ≈ 2.0
* `!calc e^2` = e² ≈ 7.3890
* `!calc log(10*10, 10)` = 2.0

**Supported Functions** 
* `sqrt(x)` - square root
* `log(x)` - natural logarithm
* `log(x,y)` - base-y logarithm of x 
* `sin(x)` - sine 
* `cos(x)` - cosine 
* `tan(x)` - tangent 
* `atan(x)` - arctangent
* `atan2(y,x)` - Sector aware arc tangent. 
* `factorial(n)` - n!
* `comb(n, k)` - Binomial coefficient n over k 
* `rad(degrees)` - Convert degrees to radians
* `deg(radians)` - Convert radians to degrees
* `norm(*x)` - Compute the length of vector x.

**Constants**
* `pi`, `e`, `inf`
* integer and floating point numbers 

NB: exponents in floating point numbers, e.g. `1.2e-3` are not yet supported due to parsing problems
