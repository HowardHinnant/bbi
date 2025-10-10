# Better Behaved Integers (bbi)

## Overview

The purpose of this header-only C++20 library is to provide integral arithmetic
with greatly reduced undefined behavior. Additionally some of the more
irritating behavior of C and C++ integers is made a little more civilized.

For example, there is no integer promotion—that is, the sum of two 8 bit
integers is an 8 bit integer—and all implicit conversions are value-preserving.
Explicit conversions are supplied for conversions that are not value-preserving,
and comparisons between signed and unsigned types always give the right answer.
Finally, if you need an integer with twice the bit width of the one you\'re
using, this library provides it.

Example use:

```c++
#include "bbi.h"
#include <iostream>

int
main()
{
    // This exposes names of integers such as i32 (signed 32 bit integral type)
    using namespace bbi::wrap;

    i32 x = 2;
    i32 y = 3;
    i32 z = x + y;
    std::cout << x << " + " << y << " = " << z << '\n';
}
```

_Output:_

```
2 + 3 = 5
```

So, nothing special, right?

For the most part, right! This library should dissolve into the background and
become nearly invisible. However, there are corner cases where the C++ language
can wreak havoc on your code when dealing with integral types. The rest of this
document seeks to address that problem by demonstrating what damage can be done,
and how this library intends to diagnose and potentially fix it.

## Modes

There are 5 behavioral modes which determine what happens on overflow: `wrap`,
`sat`, `thrw`, `term`, and `raw`. They are most easily accessed with a using
directive, for example:

```c++
using namespace bbi::wrap;
```

When this is issued, the following types are available to model signed and
unsigned integers with the indicated bit width:

```
i8
i16
i32
i64
i128
i256
i512
i1024
i2048
i4096

u8
u16
u32
u64
u128
u256
u512
u1024
u2048
u4096
```

Each of these are a type alias to a specialization of

```c++
template <SignTag S, unsigned N, Policy P> class bbi::Z;
```

Where `SignTag` can be `bbi::Signed` or `bbi::Unsigned`, `N` can be any power of
2 that is greater than or equal to 8 (bounded by your compiler\'s memory limits,
and your patience for performance), and `Policy` is one of `bbi::Wrap`,
`bbi::Saturate`, `bbi::Terminate`, or `bbi::Throw`.

Each Policy behaves just like the built-in integral types, except that there is no integral
promotion and there is no undefined behavior when overflow occurs: the policy dictates what the
overflow behavior should be.

### wrap

This is modulo 2<sup>n</sup> arithmetic where n is the bit width. This is also
known as two\'s complement. The difference between this and C++\'s arithmetic is
that signed overflow is well-defined.

_Example:_

```c++
#include "bbi.h"
#include <iostream>

int
main()
{
    using namespace bbi::wrap;

    i8 x{100};
    std::cout << x*x << '\n';
}
```

_Output_:

```
16
```

_Note:_ `i8` is a numeric type, not a character type. This is 100*100 computed
with infinite precision but then truncated back into 8 bits (10,000 % 128).

_Note:_ Division by zero, and mod 0 when using `wrap` remains undefined behavior.
However in other modes this behavior is defined (to saturate, throw or terminate).

### sat

On overflow saturate to either the minimum or maximum representable value, whichever
is the closest to the exact answer.

_Example:_

```c++
#include "bbi.h"
#include <iostream>

int
main()
{
    using namespace bbi::sat;

    i8 x{100};
    std::cout << x*x << '\n';
}
```

_Output_:

```
127
```

_Note:_ This is 100*100 computed with infinite precision but then truncated back
to the closet representable value in `i8`.

_Note:_ Division by zero saturates to either the minimum or maximum value,
depending on sign.  Mod (`operator%`) by zero is an expensive no-op in this
mode.  The modulus `x % y` is computed by `x - x/y*y`.  When `y` is 0 the
second term is `0`, and the result is `x`.

### thrw

On overflow throw a `std::overflow_error`.

_Example:_

```c++
#include "bbi.h"
#include <iostream>

int
main()
{
    using namespace bbi::thrw;

    i8 x{100};
    std::cout << x*x << '\n';
}
```

_Output_:

```
terminating due to uncaught exception of type std::overflow_error:
    Z<Signed, 8, Throw>(100) * Z<Signed, 8, Throw>(100) overflowed
```

_Note:_ `i8` is a type alias for `Z<Signed, 8, Throw>`.

### term

On overflow `std::terminate` after an informative message to `std::cerr`.

_Example:_

```c++
#include "bbi.h"
#include <iostream>

int
main()
{
    using namespace bbi::term;

    i8 x{100};
    std::cout << x*x << '\n';
}
```

_Output_:

```
Z<Signed, 8, Terminate>{100) * Z<Signed, 8, Terminate>{100) overflowed
libc++abi: terminating
```

### raw

The bbi integral types alias the corresponding standard integral types such as
`std::int8_t`. This allows one to easily switch between using this library for
debugging and testing, and using the standard integral types for release code.

The following types don\'t exist in this namespace: `i128`, `i256`, `i512`, `i1024`,
`u128`, `u256`, `u512`, and `u1024`.

_Example:_

```c++
#include "bbi.h"
#include <iostream>

int
main()
{
    using namespace bbi::raw;

    i8 x{100};
    std::cout << x*x << '\n';
}
```

_Output_:

```
10000
```

_Note:_ Integral promotion to `int` occurred.  Note that in the other modes,
**no** integral promotion occurred.

### Arithmetic Summary

The above examples all used the same type for both operands.  When the operands
are of different types, but have the same Policy, they can be operated on as if both
operands are first brought to a common type using `std::common_type`.

## Unary negation

In `wrap` mode this operation will negate and wrap on overflow.  If the type is
signed, negating the minimum value results in the same value (without undefined behavior).

In `sat` mode this operation will negate as if a signed infinite precision type and then
truncate to the minimum or maximum if the result is out of range.  For example if the
type is unsigned, the result is always 0.  And if the type is signed and the value is
the minimum, the result is the maximum.

In the `thrw` and `term` modes this operation will negate as if a signed infinite precision type and then
if the result is out of range, throw or terminate respectively.  For example, if the type
is unsigned, negating any value but 0 will throw or terminate.  And if the type is
signed, negating the minimum value will throw or terminate.

## Conversions

### Explicit conversions

All bbi integral types (signed and unsigned, any bit width, and any Policy) can
be explicitly converted to one another, and to/from C++ integral types.
However, if the destination has a Throw or Terminate Policy, and if the run-time value
of the source can not be preserved, then an exception is thrown or `terminate` is called.

_Example:_

```c++
#include "bbi.h"
#include <iostream>

int
main()
{
    using namespace bbi::wrap;

    i64 x = 0x7FFF'FFFF'FFFF'FFFF;
    i32 y{x};
    std::cout << y << '\n';
}
```

_Output_:

```
-1
```

_Example:_

```c++
#include "bbi.h"
#include <iostream>

int
main()
{
    using namespace bbi::thrw;

    i64 x = 0x7FFF'FFFF'FFFF'FFFF;
    i32 y{x};
    std::cout << y << '\n';
}
```

_Output_:

```
terminating due to uncaught exception of type std::overflow_error:
    Z<Signed, 32, Throw>(Z<Signed, 64, Throw>(9223372036854775807)) overflowed
```

An explicit conversion from `std::string_view` to a bbi integral type is allowed,
as well as an explicit conversion from a bbi integral type to `std::string`.  This
facilitates I/O.

_Example:_

```c++
#include "bbi.h"
#include <iostream>
#include <string>
#include <string_view>

int
main()
{
    using namespace bbi::term;
    i8 i{"127"};
    std::cout << i << " as string: " << std::string{i} << '\n';
    i8 j{"128"};
}
```

_Output_:

```
127 as string: 127
Z<Signed, 8, Terminate>{120} + Z<Signed, 8, Terminate>{8} overflowed
libc++abi: terminating
```

### Implicit conversions

Type `bbi::X` can be implicitly converted to type `bbi::Y` if all representable
values of `X` can be represented by `Y`, and if `X` and `Y` have the same
Policy.

_Example:_

```c++
i8  i{};
u32 u = i;
```

_Output_:

```
error: no viable conversion from 'Z<bbi::Signed, 8, [...]>' to 'Z<bbi::Unsigned, 32, [...]>'
    u32 u = i;
        ^   ~
```

_Note:_ Even though `u` has lots more bits than `i`, `u` can\'t represent
negative values that `i` may have. Therefore an implicit conversion is not
allowed.

_Corollary:_ Implicit conversions from C++ standard integral types to bbi integral types
are allowed only if the bbi integral type can represent all values of the standard
integral type.  Note, the set of standard integral types excludes `bool` and character
types.  `signed char` and `unsigned char` are standard integral types, not character
types.  For those built-in integral types that are not standard integral types (`bool`
and character types) only explicit conversion is allowed.

_Example:_

```c++
i8 x = 0;  // compile-time error: 0 has type int that could hold values that don't fit in 8 bits
i8 y = std::int8_t{0};  // ok
```

## std::common_type

When two or more bbi integers have the same Policy, they are fully integrated
into the `std::common_type` infrastructure. Given two such integers `X` and `Y`,
`std::common_type_t<X, Y>` exists, and both `X` and `Y` are implicitly
convertible to `std::common_type_t<X, Y>`. From this it follows that
`std::common_type_t<X, Y>` can represent the union of the values representable
by `X` and `Y`.  If `X` or `Y` (but not both) is a standard integral type, then
the result is the same as if the integral type is converted to a bbi integral
type with the same sign, bit width as integral type, and the same policy as the
bbi integral type, and then the `common_type` is found.  If `X` and `Y` are bbi
types with different Policies, there is no `common_type`, and if attempted, a
compile-time error will result.

_Examples:_

| `X` | `Y` | `common_type_t<X, Y>` |
| --- | --- | :---: |
| `i8` | `i16` | `i16` | 
| `u8` | `u16` | `u16` | 
| `i8` | `u8` | `i16` | 
| `i8` | `u16` | `i32` | 
| `i16` | `u8` | `i16` | 
| `i8` | `std::uint8_t` | `i16` | 

## Mixed mode arithmetic

If two different bbi types appear in an arithmetic operation, as long as they
have the same Policy the operation will proceed normally after converting both to their
`common_type`.  If one of the operands is a standard
integral type, it will first be converted to the integral\'s equivalent bbi
integral type with the same Policy as the other operand.

## Comparisons

bbi integral types support all 6 comparison operators, and as long as the two
operands have the same Policy, two different bbi types can be compared. The
operation will proceed normally after converting both to their `common_type`.
Note that this means that comparisons **always** give the correct
result, unlike in C and C++ when comparing signed and unsigned types.  For
example a signed type with a negative value will _always_ compare less than an
unsigned type, no matter the bit width of either operand. Comparisons with
standard integral types are also allowed and proceed as if the standard integral
type is converted to the equivalent bbi integral type with the same Policy as
the other operand.

## Bitwise operations

### Shift operators

The right operand can be the identical bbi type as the left operand or a
built-in integral type.  If the right operand is a bbi type, it is converted to
`int` before proceeding.

#### Left shift

The left shift operator for `Wrap` wraps the rhs `int` operand such that it
collapses to the range `[0, N-1]` where `N` is the bit width of the left operand. 
No value for the right operand is too high.  The left operand will be shifted
left in the range of `[0, N-1]` bits.

The left shift operator for `Saturate` performs a left shift when the right
operand is in the range `[0, N-1]` where `N` is the bit width of the left operand. 
If the right operand is negative a right shift is performed.  If the right
operand is greater than `N-1`, `0` is returned.

The left shift operator for `Throw` performs a left shift when the right operand
is in the range `[0, N-1]` where `N` is the bit width of the left operand. 
Otherwise a `std::overflow_error` is thrown.

The left shift operator for `Terminate` performs a left shift when the right
operand is in the range `[0, N-1]` where `N` is the bit width of the left operand. 
Otherwise `terminate` is called.

#### Right shift

The right shift operator for `Wrap` wraps the rhs `int` operand such that it
collapses to the range `[0, N-1]` where `N` is the bit width of the left
operand. No value for the right operand is too high.  The left operand will be
shifted right in the range of `[0, N-1]` bits.  If the left operand is signed
and the value is negative, 1\'s fill the vacated high order bits, otherwise 0\'s
fill the vacated high order bits.

The right shift operator for `Saturate` performs a right shift when the right
operand is in the range `[0, N-1]` where `N` is the bit width of the left operand. 
If the right operand is negative a left shift is performed.  If the right
operand is greater than `N-1` and the type is signed and the value is negative,
all 1 bits are returned; else all 0 bits are returned.

The right shift operator for `Throw` performs a right shift when the right
operand is in the range `[0, N-1]` where `N` is the bit width of the left operand. 
Otherwise a `std::overflow_error` is thrown.

The right shift operator for `Terminate` performs a right shift when the right
operand is in the range `[0, N-1]` where `N` is the bit width of the left operand. 
Otherwise `terminate` is called.

### Bitwise not

This operation negates every bit in a copy of the operand and returns it.  The
Policy makes no difference in behavior and there is never a failure.

### Bitwise and

This operation only works when both operands have identical bbi types.  Each
pair of bits is bitwise and\'ed into a copy and returned.  The Policy has no
impact on the behavior of this operation.

### Bitwise or

This operation only works when both operands have identical bbi types.  Each
pair of bits is bitwise or\'ed into a copy and returned.  The Policy has no
impact on the behavior of this operation.

### Bitwise exclusive or

This operation only works when both operands have identical bbi types.  Each
pair of bits is bitwise exclusive or\'ed into a copy and returned.  The Policy
has no impact on the behavior of this operation.

## Quotient / Remainder algorithms

There are four namespace scope functions that can take two signed bbi integers with
the same policy and return the quotient and remainder in a `bbi::div_t`.  These
functions support _truncated_ division, _floored_ division, _ceiling_ division,
and _Euclidean_ division as described [here](https://en.wikipedia.org/wiki/Modulo).
 The function names are `trunc_div`, `floor_div`, `ceil_div` and `euc_div`.

The remainder of all four functions follows the invariant that `r == n - q*d`.
For Euclidean division, the remainder is always non-negative.

The only failure mode for these functions is division by 0 for `wrap`, `thrw` and
`term` modes, and division of the minimum value by -1 for `thrw` and `term` modes.

_Example:_

```c++
i32 n = 10;
i32 d = 3;
std::cout << n << '/' << d << '\n';
std::cout << "trunc_div : " << trunc_div(n, d) << '\n';
std::cout << "floor_div : " << floor_div(n, d) << '\n';
std::cout << "ceil_div  : " << ceil_div(n, d) << '\n';
std::cout << "euc_div   : " << euc_div(n, d) << "\n\n";

n = -10;
d = 3;
std::cout << n << '/' << d << '\n';
std::cout << "trunc_div : " << trunc_div(n, d) << '\n';
std::cout << "floor_div : " << floor_div(n, d) << '\n';
std::cout << "ceil_div  : " << ceil_div(n, d) << '\n';
std::cout << "euc_div   : " << euc_div(n, d) << "\n\n";

n = 10;
d = -3;
std::cout << n << '/' << d << '\n';
std::cout << "trunc_div : " << trunc_div(n, d) << '\n';
std::cout << "floor_div : " << floor_div(n, d) << '\n';
std::cout << "ceil_div  : " << ceil_div(n, d) << '\n';
std::cout << "euc_div   : " << euc_div(n, d) << "\n\n";

n = -10;
d = -3;
std::cout << n << '/' << d << '\n';
std::cout << "trunc_div : " << trunc_div(n, d) << '\n';
std::cout << "floor_div : " << floor_div(n, d) << '\n';
std::cout << "ceil_div  : " << ceil_div(n, d) << '\n';
std::cout << "euc_div   : " << euc_div(n, d) << '\n';
```

_Output:_

```
10/3
trunc_div : {3, 1}
floor_div : {3, 1}
ceil_div  : {4, -2}
euc_div   : {3, 1}

-10/3
trunc_div : {-3, -1}
floor_div : {-4, 2}
ceil_div  : {-3, -1}
euc_div   : {-4, 2}

10/-3
trunc_div : {-3, 1}
floor_div : {-4, -2}
ceil_div  : {-3, 1}
euc_div   : {-3, 1}

-10/-3
trunc_div : {3, -1}
floor_div : {3, -1}
ceil_div  : {4, 2}
euc_div   : {4, 2}
```

## Requirements

Requires C++20 and:

* gcc 10.1 or later
* clang 17.0.1 or later (Apple clang 16.0.0 or later)
* VS 2022 17.14 or later (not yet released as of Jan 1, 2025)
