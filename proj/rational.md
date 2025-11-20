# Rational types

## Overview

This is an example project showing how the `bbi` integral library can be used to create
another useful library.  In this case, a rational library:

```c++
namespace bbi
{

template <unsigned N> class rational;

}
```

The template parameter `N` can take on values of power of 2 between 8 and 4096, inclusive.

The upper values are stretching the capabilities of compilers, especially in the
compile-time department.  On clang, use of `-fconstexpr-steps=4294967295` may be used to
increase the ability of the complier to compute things at compile time.  However this may
also have the negative effect of increasing both compile times, and the run times of the
resultant executable.  Use with caution.

`rational<N>` is composed of two signed `bbi` integral values with a bit width of `N`,
holding the numerator and denominator.  The default is to have the overflow behavior of
these underlying integral types to be `Terminate`.  However when `NDEBUG` is defined, the
overflow policy switches to `Wrap`.  If `terminate` is called within `rational`, this
represents a bug in `rational`.

No operation on a `rational` should cause the numerator or denominator to overflow.
Instead the `rational` can hold values of +/- infinity and nan.  Therefore a future
release may only support `Wrap`.

This `rational` library differs from others in that numerator and denominator overflow is
always avoided.  If the exact answer won\'t fit in the numerator or denominator, the
closest fraction where the numerator and denominator fit in the resultant type is chosen. 
This is similar to \"round to nearest\" for floating point expressions where the result
won\'t fit exactly into the result type.

For example, with the boost rational library (and others I\'m aware of):

```c++
#include "boost/rational.hpp"
#include <cstdint>
#include <iostream>

int
main()
{
    using R = boost::rational<std::int16_t>;
    R r1{63, 50};
    R r2{5, 72};
    auto r3 = r1 + r2;
    std::cout << +r1.numerator() << '/' << +r1.denominator() << " + "
              << +r2.numerator() << '/' << +r2.denominator() << " = "
              << +r3.numerator() << '/' << +r3.denominator() << " or about "
              << (double)r3.numerator()/r3.denominator() << '\n';
}
```

This outputs:

```
63/50 + 5/72 = 2393/1800 or about 1.32944
```

The print statement is messier than it needs to be, but ignore that for the moment.

The arithmetic above is exact, and gets the exact right answer because `int16_t` can
handle the resultant numerator and denominator.

But changing this to 8 bits:

```c++
    using R = boost::rational<std::int8_t>;
```

Results in:

```
63/50 + 5/72 = 89/8 or about 11.125
```

The numerator and denominator simply overflow and result in a garbage answer.

Running this same experiment with this library:

```c++
#include "proj/rational.h"
#include <cstdint>
#include <iostream>

int
main()
{
    using R = bbi::rational<16>;
    R r1{63, 50};
    R r2{5, 72};
    auto r3 = r1 + r2;
    std::cout << r1 << " + " << r2 << " = " << r3 << " or about "
              << (double)(int)r3.num()/(int)r3.den() << '\n';
}
```

At 16 bits the output is the same as the boost library:

```
63/50 + 5/72 = 2393/1800 or about 1.32944
```

But change to 8 bits:

```c++
    using R = bbi::rational<8>;
```

and the output is:

```
63/50 + 5/72 = 113/85 or about 1.32941
```

The output isn\'t exact, but it is the closest possible representation using signed 8 bit
integers.  The error is about `0.000032679738562`.

This means that `bbi::rational` can be used pretty much like floating point types.  The
result is exact if that is possible, but rounded to the closest representable value if
exact is not possible.  And if the resulting error is too large, you can always double the
precision until the error is acceptable (up to 4096 bits).  In this particular example, 16
bits (total of 32 bits counting the size of both numerator and denominator) are enough for
an exact answer.

This all works by performing the four basic operations (`+` `-` `*` `/`) in double
precision and then converting that double precision result back to the original precision
using continued fractions to find the closest representable value.  There is a performance
cost to this.  But the freedom to treat rationals like a floating point opens up a ton of
functionality.

For example:

```c++
#include "proj/rational.h"
#include <iostream>

int
main()
{
    using R = bbi::rational<8>;
    R r1{63, 50};
    auto r2 = r1 * r1;
    std::cout << r1 << " squared is " << r2 << " or about "
              << (double)(int)r2.num()/(int)r2.den() << '\n';
    std::cout << "sqrt(" << r2 << ") = " << sqrt(r2) << '\n';
}
```

Output:

```
63/50 squared is 127/80 or about 1.5875
sqrt(127/80) = 63/50
```

We squared an 8 bit rational, got a best approximation result, and then took the square
root of that, and got back the original value.

Notes:

*  We took the `sqrt` of a `rational`!
*  Even though both the square and square root produced approximate values, they were
close enough to correct that the round trip was lossless.  That lossless property isn\'t
guaranteed, but it does demonstrate how good the approximations are.

Running the experiment again, but at 16 bit precision, produces the exact answers:

```
63/50 squared is 3969/2500 or about 1.5876
sqrt(3969/2500) = 63/50
```

## Invariants

No matter the inputs upon construction, or the operation performed, a `rational` object
maintains the following invariants:

Let `M = std::numeric_limits<V>::max()`, wehre `V` is the type of both the numerator and
denominator in the `rational`.

Let `r` be an object of type `rational<N>`.

* `r.den() >= 0`.
* If r.den() == 0, `r.num()` will be one of `1`, `-1`, or `0`, with `r` representing
infinity, -infinity or nan respectively.
* If `r.den() > 0`, then `r.num()` will be in the range of `[-M, M]`.  Note that neither
`r.num()` nor `r.den()` can ever equal `std::numeric_limits<V>::min()`.  This restriction
assures that both the additive inverse and the multiplicative inverse, done twice, always
results in the orignal value.
* If `r.num() == 0`, then either `r.den() == 1`, representing `0`, or `r.den() == 0`,
representing nan.
* If `r.den() != 0`, then `gcd(r.num(), r.den()) == 1`.

Arithmetic with infinities and nan follow the same rules as IEEE floating point.

There is no signed zero.

The range of representable finite values is `[-M, M]`.  If the exact answer to a
computation is outside of `(-M - 1/2, M + 1/2)`, an infinity will result.  Otherwise the
exact answer will be rounded to a finite value in the range `[-M, M]`.  At
`abs(r) == M + 1/2` the algorithm breaks the tie with round-towards-even which favors
+/- infinity.

## Conversions

Conversions are allowed between `rational<N1>` and `rational<N2>`.  All conversions
follow the usual conventions for `bbi` integers:

If the conversion is guaranteed to be exact, i.e. from low precision to higher precision,
then the conversion is implicit, otherwise it is explicit.  Explicit conversions perform
a round-to-nearest approximation and never result in an infinity or nan, unless the source
is outside of the representable range of the destination, or the source itself is an
infinity or nan.

Conversions from standard integers and `bbi` integers are allowed.  If those conversions
are guaranteed to be lossless, they are implicit, else they are explicit.

Explicit conversions from `std::string_view` are allowed assuming the string is of one
of the following forms:

* `"numerator"` : creates a value with a numerator of the indicated value.  The
denominator is set to 1.
* `"numerator/denominator"` : creates a value with a numerator of the indicated value and
denominator of the indicated values.  They will be reduced to lowest terms during the
construction.
* `"[a0]"` : creates a value with `a0` as the numerator and 1 as the denominator.
* `"[a0;]"` : creates a value with `a0` as the numerator and 1 as the denominator.
* `"[a0; a1, a2, ...]"` : creates a value equivalent to the simple continued fraction
`[a0; a1, a2, ...]`.  White space is ignored.
* `"inf"` : creates infinity
* `"-inf"` : creates -infinity
* `"nan"` : creates nan

Explicit conversions to `std::string` are allowed and follow the syntax of the explicit
conversions from `std::string_view`.

Conversions from `std::ratio` to `rational` are provided.  The conversion is implicit
if value-preserving else explicit.  Conversions in the reverse direction must be handled
by explicit converions of the numerator and denominator to `std::intmax_t`.

_Example:_

```c++
#include "proj/rational.h"
#include <iostream>

int
main()
{
    bbi::rational<32> constexpr r = std::micro{};
    std::cout << r << '\n';
    using R = std::ratio<std::intmax_t{r.num()}, std::intmax_t{r.den()}>;
    std::cout << R::num << '/' << R::den << '\n';
}
```

_Output:_

```
1/1000000
1/1000000
```

## Other constructors

The default constructor results in a `nan` value.  Such values remain as nan until
assigned to with a non-nan value.

A two argument constructor is available which takes the numerator and denominator.  The
types of these arguments can be standard integral types or `bbi` integral types.  If
both arguments are implicitly convertible to the type of the numerator and denominator
(know as `value_type`), then the construction is implicit, else it is explicit.  If the
construction is explicit, the arguments will form an intermediate exact higher-precision
`rational` which is then rounded to the nearest representable value in `*this`.

_Example_:

```c++
#include "proj/rational.h"
#include <iostream>

int
main()
{
    using R = bbi::rational<8>;
    R r1{600, 501};
    std::cout << r1 << '\n';
}
```

_Output_:

```
103/86
```

_Challenge:_

Find a fraction with numerator in the range `[-127, 127]` and denominator in the range
`[0, 127]`, whose absolute difference from <sup>600</sup>/<sub>501</sub> is less than
`abs(`<sup>600</sup>/<sub>501</sub> - <sup>103</sup>/<sub>86</sub>`)`.  That difference,
by the way, is <sup>1</sup>/<sub>14362</sub>.

_Conjecture:_

You won\'t find one.

## Arithmetic

All four of the basic arithmetic operations are provided: `+`, `-`, `*`, `/`.  Additionally
the assignment variants are provided (e.g. `+=`).  Unary `+`, `-` are provided and are
always exact.  The increment and decrement operators are provided and have the expected
semantics.

Mixed mode arithmetic is allowed and always results in the `std::common_type` of the
two operands (i.e. the one with the larger bit width).

## Comparisons

All 6 comparison operators are provided and are always exact.  Mixed mode is supported
with different precisions and with either operand being a standard or `bbi` integer type.

## Algorithms

### Basic

`abs` returns the absolute value of a `rational`.  It is always exact.

`reciprocal` is provided both as a non-`const` member function, and as a namespace-scope
function:

```c++
rational<8> r{1, 5};
r.reciprocal();           // r == 5;
auto r2 = reciprocal(r);  // r2 == 1/5, r == 5
```

The `reciprocal` function is always exact.

### Power functions

```c++
template <unsigned N, unsigned N2, Policy P>
constexpr
rational<N>
root(rational<N> f, Z<Unsigned, N2, P> du);
```

This caculates the root of the `rational` `f`, returning it as the same type.  The second
argument can be either an unsigned `bbi` integral type, or an unsigned standard integral
type.  This uses a Newton iteration that ends when the result is either not changing,
or bouncing between two values.  The result is in general approximate.  If the exact
result is not real, nan is returned.  Infinity can also be returned.  IEEE procedures are
followed for special arguments.

```c++
template <class Num, unsigned N, Policy P>
constexpr
Num
power(Num const& x, Z<Signed, N, P> n);
```

This can raise anything to a signed integral power.  If the second argument is negative,
it results in the inverse of the equivalent unsigned integral power.

```c++
template <unsigned Nx, unsigned Ny>
constexpr
auto
power(rational<Nx> const& x, rational<Ny> const& y);
```

This raises a rational to a rational power.  This is a convenience function for finding
the root of `x` to `y.den()` (`y.den()` is never negative), raised to the `y.num()`
integral power.

```c++
template <unsigned N>
constexpr
auto
sqrt(rational<N> const& x);
```

This finds the square root of `x`.  It is a convenience function for:

```c++
    return root(x, 2u);
```
or:
```c++
    return power(x, rational<8>{1,2});
```

### Misc:

```c++
template <unsigned N>
constexpr
rational<N>
fac(rational<N> x);
```

Returns the factorial of `x`.  If x is not a non-negative integer, returns `nan`.  Returns
`inf` if the result is outside the range of `rational<N>`.  This is basically a
convenience function that finds use in Taylor Series applications.

```c++
template <unsigned N>
std::string
cf_string(rational<N> r);
```

Prints the value `r` as a simple continued fraction of the from `[a0; a1, a2, ...]`. This
is the inverse of the constructor that takes a `std::string_view` containing
`"[a0; a1, a2, ...]"`.

### Constants

There are a series of constants that largely follow those contants in the C++ standard.
But they are in namespace `bbi` and drop the trailing `_v`.  They are used like this:

```c++
auto constexpr pi_8 = bbi::pi<8>;  // pi_8 == rational<8>{22, 7}
auto constexpr pi_128 = bbi::pi<128>;
// pi_128 == rational<128>{
//     135383245921877291206888365157940675591
//   ,  43093825600584903152997992180848828034}
```

The complete list is:

```
pi
inv_pi
sqrt2
sqrt3
inv_sqrt3
sqrt5
sqrt7
e
ln2
log2e
ln10
log10e
inv_sqrtpi
egamma
phi
```

At 4096 bits and below they have the best rational approximation that will fit into those
bit widths.  Above 4096 they may not compile, or may not have the best approximation. At
4096, the compile time may be excessive, and may require `-fconstexpr-steps=4294967295`.

### Trigonometric functions

All six trig functions and their inverses are supported:

```c++
sin
cos
tan
cot
sec
csc
asin
acos
atan
acot
asec
acsc
```

_Example:_

```c++
#include "proj/rational.h"
#include <iostream>

int
main()
{
    using namespace bbi;
    std::cout << rational<8>{asin(reciprocal(sqrt2<8>))*180/pi<8>} << '\n';
}
```

_Output:_

```
45
```
