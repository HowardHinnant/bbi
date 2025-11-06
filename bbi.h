#ifndef BBI_H
#define BBI_H

// https://github.com/HowardHinnant/bbi

// The MIT License (MIT)
//
// Copyright (c) 2024 Howard Hinnant
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Our apologies.  When the previous paragraph was written, lowercase had not yet
// been invented (that would involve another several millennia of evolution).
// We did not mean to shout.

#include <algorithm>
#include <bit>
#include <charconv>
#include <climits>
#include <cstdint>
#include <exception>
#include <iostream>
#include <istream>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>

namespace bbi
{

// Change this to 16 on 16 bit architectures, to 32 on 32 bit architectures, etc.
#ifndef BBI_LIMIT
#  define BBI_LIMIT 64
#endif

unsigned constexpr Nlimit = BBI_LIMIT;

struct Signed{ explicit Signed() = default; };
struct Unsigned{ explicit Unsigned() = default; };

template <class T>
concept SignTag = std::is_same_v<T, Signed> || std::is_same_v<T, Unsigned>;

template <SignTag S1, SignTag S2>
inline constexpr bool operator==(S1, S2) noexcept {return std::is_same_v<S1, S2>;}

template <SignTag S1, SignTag S2>
inline constexpr bool operator!=(S1 s1, S2 s2) noexcept {return !(s1 == s2);}

template <SignTag> std::string to_string();
template <> inline std::string to_string<Signed>()   {return "Signed";}
template <> inline std::string to_string<Unsigned>() {return "Unsigned";}

struct Wrap{ explicit Wrap() = default; };
struct Saturate{ explicit Saturate() = default; };
struct Terminate{ explicit Terminate() = default; };
struct Throw{ explicit Throw() = default; };

template <class T>
concept Policy = std::is_same_v<T, Wrap> || std::is_same_v<T, Saturate> ||
                 std::is_same_v<T, Terminate> || std::is_same_v<T, Throw>;

template <Policy P1, Policy P2>
inline constexpr bool operator==(P1, P2) noexcept {return std::is_same_v<P1, P2>;}

template <Policy P1, Policy P2>
inline constexpr bool operator!=(P1 p1, P2 p2) noexcept {return !(p1 == p2);}

template <Policy> std::string to_string();
template <> inline std::string to_string<Wrap>()      {return "Wrap";}
template <> inline std::string to_string<Saturate>()  {return "Saturate";}
template <> inline std::string to_string<Throw>()     {return "Throw";}
template <> inline std::string to_string<Terminate>() {return "Terminate";}

template <class T>
concept ErrorCheckedPolicy = std::is_same_v<T, Terminate> || std::is_same_v<T, Throw>;

template <std::integral> std::string to_string();
template <> inline std::string to_string<std::uint8_t >() {return "uint8_t";}
template <> inline std::string to_string<std::uint16_t>() {return "uint16_t";}
template <> inline std::string to_string<std::uint32_t>() {return "uint32_t";}
template <> inline std::string to_string<std::uint64_t>() {return "uint64_t";}
template <> inline std::string to_string<std::int8_t >()  {return "int8_t";}
template <> inline std::string to_string<std::int16_t>()  {return "int16_t";}
template <> inline std::string to_string<std::int32_t>()  {return "int32_t";}
template <> inline std::string to_string<std::int64_t>()  {return "int64_t";}

static_assert(CHAR_BIT == 8);

// N must be a power of 2 that is 8 or larger.
// The upper limit is dictated by compiler limits or the size of unsigned.
// This could be a couple of billion bits, which is way overkill for any
// imaginable application:  2^(2^31) which is about 10^646,456,993.
// There are only about 10^80 atoms in the universe.
template <unsigned N>
concept ProperIntSize = std::has_single_bit(N) && 8 <= N &&
                                                  N <= (1u << (sizeof(N)*CHAR_BIT - 1));

static_assert(ProperIntSize<Nlimit>);

template <SignTag S, unsigned N, Policy P, bool small = N <= Nlimit>
class Z;

template <class X>
requires (SignTag<typename X::sign> && std::is_unsigned_v<decltype(X::size)>
          && Policy<typename X::policy>)
inline
std::string
to_string()
{
    return "Z<" + to_string<typename X::sign>() + ", " + std::to_string(X::size)
                   + ", " + to_string<typename X::policy>() + '>';
}

namespace detail
{

template <SignTag S1, unsigned N1, Policy P1>
constexpr
Z<S1, N1, P1>
zero_high_bit(Z<S1, N1, P1> x) noexcept;

template <SignTag S1, unsigned N1, Policy P1>
constexpr
Z<S1, N1, P1>
set_high_bit(Z<S1, N1, P1> x) noexcept;

template <SignTag S, unsigned N, Policy P, std::integral I>
constexpr
void
check(Z<S, N, P>& r, I i) noexcept (P{} != Throw{});

template <SignTag S1, unsigned N1, Policy P1, SignTag S2, unsigned N2, Policy P2>
constexpr
void
check(Z<S1, N1, P1>& r, Z<S2, N2, P2> const& x) noexcept (P1{} != Throw{});

template <unsigned N>
constexpr
Z<Unsigned, 2*N, Wrap>
mul2(Z<Unsigned, N, Wrap> const& x, Z<Unsigned, N, Wrap> const& y) noexcept;

template <unsigned N>
constexpr
Z<Signed, 2*N, Wrap>
mul2(Z<Signed, N, Wrap> const& x, Z<Signed, N, Wrap> const& y) noexcept;

template <unsigned N1>
requires (N1 > Nlimit)
void
push_digits(std::string& r, Z<Unsigned, N1, Wrap>& x);

template <unsigned N>
requires(N <= Nlimit)
constexpr
unsigned
divu10(Z<Unsigned, N, Wrap>& u) noexcept;

template <class I>
concept StandardSignedInteger = std::is_same_v<I, signed char> ||
                                std::is_same_v<I, short>       ||
                                std::is_same_v<I, int>         ||
                                std::is_same_v<I, long>        ||
                                std::is_same_v<I, long long>;

template <class I>
concept StandardUnsignedInteger = std::is_same_v<I, unsigned char>  ||
                                  std::is_same_v<I, unsigned short> ||
                                  std::is_same_v<I, unsigned>       ||
                                  std::is_same_v<I, unsigned long>  ||
                                  std::is_same_v<I, unsigned long long>;

template <class I>
concept StandardInteger = StandardSignedInteger<I> || StandardUnsignedInteger<I>;

// Is {S2, N2} -> {S1, N1} value preserving?
template <class S1, unsigned N1, class S2, unsigned N2>
concept ValuePreservingConversion =
    (S1{} == S2{} ?
        N1 >= N2 :
        S1{} == Unsigned{} ? false : N1 > N2);

template <class Z, class I>
concept ExplicitFromI =
    !StandardInteger<I> || !ValuePreservingConversion<typename Z::sign, Z::size,
                              std::conditional_t<std::is_signed_v<I>, Signed, Unsigned>,
                              sizeof(I)*CHAR_BIT>;

template <class Z, class I>
concept NoexceptFromI =
    (typename Z::policy{} != Throw{} ||
     ValuePreservingConversion<typename Z::sign, Z::size,
                               std::conditional_t<std::is_signed_v<I>, Signed, Unsigned>,
                               std::is_same_v<I, bool> ? 1u : sizeof(I)*CHAR_BIT>);

template <class Z1, class Z2>
concept ExplicitFromZ =
    (typename Z1::policy{} != typename Z2::policy{}) ||
    !ValuePreservingConversion<typename Z1::sign, Z1::size, typename Z2::sign, Z2::size>;

template <class Z1, class Z2>
concept NoexceptFromZ =
    (typename Z1::policy{} != Throw{}) ||
    ValuePreservingConversion<typename Z1::sign, Z1::size, typename Z2::sign, Z2::size>;

template <class T>
struct isz : std::false_type {};

template <SignTag S, unsigned N, Policy P, bool B>
struct isz<Z<S, N, P, B>> : std::true_type {};

template <class T>
concept isZ = isz<T>::value;

}  // namespace detail

template <SignTag S, unsigned N, Policy P>
requires (N <= Nlimit)
std::string
to_string(Z<S, N, P> const& x);

template <SignTag S, unsigned N, Policy P>
class Z<S, N, P, true>
{
public:
    using sign = S;
    static constexpr auto size = N;
    using policy = P;

private:
    using utypes = std::tuple<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t
#ifndef _MSC_VER
                                                                        , __uint128_t
#endif
                                                                                       >;
    using stypes = std::tuple< std::int8_t,  std::int16_t,  std::int32_t,  std::int64_t
#ifndef _MSC_VER
                                                                        , __int128_t
#endif
                                                                                       >;
    using urep_t = std::tuple_element_t<std::countr_zero(size)-3, utypes>;
    using srep_t = std::tuple_element_t<std::countr_zero(size)-3, stypes>;

    urep_t rep_;

public:
    using rep = std::conditional_t<sign{} == Signed{}, srep_t, urep_t>;

    Z() = default;

    // Construct from integral type.
    // Implicit if lossless, otherwise explicit
    template <std::integral I>
        constexpr
        explicit(detail::ExplicitFromI<Z, I>)
        Z(I i)
            noexcept(detail::NoexceptFromI<Z, I>)
            : rep_(i)
            {detail::check(*this, i);}

    // Construction from other Z

    template <unsigned N2, Policy P2>
        requires (N2 < size)
        explicit(detail::ExplicitFromZ<Z, Z<Unsigned, N2, P2>>)
        constexpr
        Z(Z<Unsigned, N2, P2> const& x)
            noexcept(detail::NoexceptFromZ<Z, Z<Unsigned, N2, P2>>)
            : rep_{x.rep_} {detail::check(*this, x);}

    template <unsigned N2, Policy P2>
        requires (N2 < size)
        explicit(detail::ExplicitFromZ<Z, Z<Signed, N2, P2>>)
        constexpr
        Z(Z<Signed, N2, P2> const& x)
            noexcept(detail::NoexceptFromZ<Z, Z<Signed, N2, P2>>)
            : rep_(srep_t{typename Z<Signed, N2, P2>::srep_t(x.rep_)})
              {detail::check(*this, x);}

    template <SignTag S2, Policy P2>
        requires (sign{} != S2{} || policy{} != P2{})
        explicit(detail::ExplicitFromZ<Z, Z<S2, size, P2>>)
        constexpr
        Z(Z<S2, size, P2> const& x)
            noexcept(detail::NoexceptFromZ<Z, Z<S2, size, P2>>)
            : rep_{x.rep_} {detail::check(*this, x);}

    template <SignTag S2, unsigned N2, Policy P2>
        requires (N2 > size && N2 <= Nlimit)
        constexpr
        explicit(detail::ExplicitFromZ<Z, Z<S2, N2, P2>>)
        Z(Z<S2, N2, P2> const& x)
            noexcept(detail::NoexceptFromZ<Z, Z<S2, N2, P2>>)
            : rep_(x.rep_) {detail::check(*this, x);}

    template <SignTag S2, unsigned N2, Policy P2>
        requires (N2 == size*2 && Nlimit < N2)
        constexpr
        explicit(detail::ExplicitFromZ<Z, Z<S2, N2, P2>>)
        Z(Z<S2, N2, P2> const& x)
            noexcept(detail::NoexceptFromZ<Z, Z<S2, N2, P2>>)
            : rep_(x.lo_) {detail::check(*this, x);}

    template <SignTag S2, unsigned N2, Policy P2>
        requires (N2 > size*2 && Nlimit < N2)
        constexpr
        explicit(detail::ExplicitFromZ<Z, Z<S2, N2, P2>>)
        Z(Z<S2, N2, P2> const& x)
            noexcept(detail::NoexceptFromZ<Z, Z<S2, N2, P2>>)
            : Z{Z<S2, size*2, policy>{x}} {}

    explicit constexpr Z(std::string_view s);
    explicit operator std::string() const;

    constexpr explicit operator bool() const noexcept {return bool(rep_);}

    template <std::integral I>
        constexpr explicit operator I() const noexcept {return I(rep(rep_));}

    constexpr Z operator+() noexcept {return *this;}
    constexpr Z& operator++() noexcept(policy{} != Throw{})
        {*this += Z{1}; return *this;}
    constexpr Z operator++(int) noexcept(policy{} != Throw{})
        {auto t(*this); ++(*this); return t;}

    constexpr Z& operator--() noexcept(policy{} != Throw{})
        {*this -= Z{1}; return *this;}
    constexpr Z operator--(int) noexcept(policy{} != Throw{})
        {auto t(*this); --(*this); return t;}

    constexpr Z& operator+=(Z const& x) noexcept(policy{} != Throw{})
        {*this = *this + x; return *this;}
    constexpr Z& operator-=(Z const& x) noexcept(policy{} != Throw{})
        {*this = *this - x; return *this;}
    constexpr Z& operator*=(Z const& x) noexcept(policy{} != Throw{})
        {*this = *this * x; return *this;}
    constexpr Z& operator/=(Z const& x) noexcept(policy{} != Throw{})
        {*this = *this / x; return *this;}
    constexpr Z& operator%=(Z const& x) noexcept(policy{} != Throw{})
        {*this = *this % x; return *this;}
    constexpr Z& operator<<=(int n) noexcept(policy{} != Throw{})
        {*this = *this << n; return *this;}
    constexpr Z& operator<<=(Z const& n) noexcept(policy{} != Throw{})
        {*this = *this << n; return *this;}
    constexpr Z& operator>>=(int n) noexcept(policy{} != Throw{})
        {*this = *this >> n; return *this;}
    constexpr Z& operator>>=(Z const& n) noexcept(policy{} != Throw{})
        {*this = *this >> n; return *this;}
    constexpr Z& operator&=(Z const& y) noexcept(policy{} != Throw{})
        {*this = *this & y; return *this;}
    constexpr Z& operator^=(Z const& y) noexcept(policy{} != Throw{})
        {*this = *this ^ y; return *this;}
    constexpr Z& operator|=(Z const& y) noexcept(policy{} != Throw{})
        {*this = *this | y; return *this;}

private:
    template <SignTag S2, unsigned N2, Policy P2, bool>
    friend
    class Z;

    template <SignTag S1, unsigned N1, Policy P1>
    friend
    constexpr
    Z<S1, N1, P1>
    detail::zero_high_bit(Z<S1, N1, P1> x) noexcept;

    template <SignTag S1, unsigned N1, Policy P1>
    friend
    constexpr
    Z<S1, N1, P1>
    detail::set_high_bit(Z<S1, N1, P1> x) noexcept;

    template <SignTag S1, unsigned N1, Policy P1, std::integral I>
    friend
    constexpr
    void
    detail::check(Z<S1, N1, P1>& r, I i) noexcept (P1{} != Throw{});

    template <SignTag S1, unsigned N1, Policy P1, SignTag S2, unsigned N2, Policy P2>
    friend
    constexpr
    void
    detail::check(Z<S1, N1, P1>& r, Z<S2, N2, P2> const& x)
        noexcept (P1{} != Throw{});

    template <SignTag S2, unsigned N2, Policy P2>
    friend
    constexpr
    bool
    operator==(Z<S2, N2, P2> const& x, Z<S2, N2, P2> const& y) noexcept;

    template <SignTag S2, unsigned N2, Policy P2>
    friend
    constexpr
    bool
    operator<(Z<S2, N2, P2> const& x, Z<S2, N2, P2> const& y) noexcept;

    template <SignTag S2, unsigned N2>
    friend
    constexpr
    Z<S2, N2, Wrap>
    operator<<(Z<S2, N2, Wrap> const& x, int n) noexcept;

    template <SignTag S2, unsigned N2>
    friend
    constexpr
    Z<S2, N2, Saturate>
    operator<<(Z<S2, N2, Saturate> const& x, int n) noexcept;

    template <SignTag S2, unsigned N2, ErrorCheckedPolicy P2>
    friend
    constexpr
    Z<S2, N2, P2>
    operator<<(Z<S2, N2, P2> const& x, int n) noexcept(P2{} == Terminate{});

    template <SignTag S2, unsigned N2>
    friend
    constexpr
    Z<S2, N2, Wrap>
    operator>>(Z<S2, N2, Wrap> const& x, int n) noexcept;

    template <SignTag S2, unsigned N2>
    friend
    constexpr
    Z<S2, N2, Saturate>
    operator>>(Z<S2, N2, Saturate> const& x, int n) noexcept;

    template <SignTag S2, unsigned N2, ErrorCheckedPolicy P2>
    friend
    constexpr
    Z<S2, N2, P2>
    operator>>(Z<S2, N2, P2> const& x, int n) noexcept(P2{} == Terminate{});

    template <SignTag S2, unsigned N2, Policy P2>
    friend
    constexpr
    Z<S2, N2, P2>
    operator~(Z<S2, N2, P2> const& x) noexcept;

    template <SignTag S2, unsigned N2, Policy P2>
    friend
    constexpr
    Z<S2, N2, P2>
    operator&(Z<S2, N2, P2> const& x, Z<S2, N2, P2> const& y) noexcept;

    template <SignTag S2, unsigned N2, Policy P2>
    friend
    constexpr
    Z<S2, N2, P2>
    operator^(Z<S2, N2, P2> const& x, Z<S2, N2, P2> const& y) noexcept;

    template <SignTag S2, unsigned N2, Policy P2>
    friend
    constexpr
    Z<S2, N2, P2>
    operator|(Z<S2, N2, P2> const& x, Z<S2, N2, P2> const& y) noexcept;

    template <SignTag S2, unsigned N2>
    friend
    constexpr
    auto
    operator+(Z<S2, N2, Wrap> const& x, Z<S2, N2, Wrap> const& y) noexcept
        -> Z<S2, N2, Wrap>;

    template <SignTag S2, unsigned N2>
    friend
    constexpr
    auto
    operator+(Z<S2, N2, Saturate> const& x, Z<S2, N2, Saturate> const& y) noexcept
        -> Z<S2, N2, Saturate>;

    template <SignTag S2, unsigned N2, ErrorCheckedPolicy P2>
    friend
    constexpr
    auto
    operator+(Z<S2, N2, P2> const& x, Z<S2, N2, P2> const& y)
        noexcept(P2{} == Terminate{})
        -> Z<S2, N2, P2>;

    template <SignTag S2, unsigned N2>
    friend
    constexpr
    auto
    operator-(Z<S2, N2, Wrap> const& x, Z<S2, N2, Wrap> const& y) noexcept
        -> Z<S2, N2, Wrap>;

    template <SignTag S2, unsigned N2>
    friend
    constexpr
    auto
    operator-(Z<S2, N2, Wrap> const& x) noexcept
        -> Z<S2, N2, Wrap>;

    template <SignTag S2, unsigned N2>
    friend
    constexpr
    auto
    operator-(Z<S2, N2, Saturate> const& x, Z<S2, N2, Saturate> const& y) noexcept
        -> Z<S2, N2, Saturate>;

    template <SignTag S2, unsigned N2>
    friend
    constexpr
    auto
    operator-(Z<S2, N2, Saturate> const& x) noexcept
        -> Z<S2, N2, Saturate>;

    template <SignTag S2, unsigned N2, ErrorCheckedPolicy P2>
    friend
    constexpr
    auto
    operator-(Z<S2, N2, P2> const& x, Z<S2, N2, P2> const& y)
        noexcept(P2{} == Terminate{})
        -> Z<S2, N2, P2>;

    template <SignTag S2, unsigned N2, ErrorCheckedPolicy P2>
    friend
    constexpr
    auto
    operator-(Z<S2, N2, P2> const& x) noexcept(P2{} == Terminate{})
        -> Z<S2, N2, P2>;

    template <SignTag S2, unsigned N2>
    friend
    constexpr
    auto
    operator*(Z<S2, N2, Wrap> const& x, Z<S2, N2, Wrap> const& y) noexcept
        -> Z<S2, N2, Wrap>;

    template <unsigned N2>
    friend
    constexpr
    Z<Unsigned, 2*N2, Wrap>
    detail::mul2(Z<Unsigned, N2, Wrap> const& x, Z<Unsigned, N2, Wrap> const& y)
        noexcept;

    template <unsigned N2>
    friend
    constexpr
    Z<Signed, 2*N2, Wrap>
    detail::mul2(Z<Signed, N2, Wrap> const& x, Z<Signed, N2, Wrap> const& y)
        noexcept;

    template <unsigned N2>
    friend
    constexpr
    auto
    operator/(Z<Unsigned, N2, Wrap> const& x,
              Z<Unsigned, N2, Wrap> const& y) noexcept
        -> Z<Unsigned, N2, Wrap>;

    template <unsigned N2>
    friend
    constexpr
    auto
    operator/(Z<Signed, N2, Wrap> const& x, Z<Signed, N2, Wrap> const& y) noexcept
        -> Z<Signed, N2, Wrap>;

    template <unsigned N2, Policy P2>
    friend
    constexpr
    unsigned
    countl_zero(Z<Unsigned, N2, P2> const& x) noexcept;

    template <unsigned N2, Policy P2>
    friend
    constexpr
    unsigned
    countr_zero(Z<Unsigned, N2, P2> const& x) noexcept;

    template <unsigned N2, Policy P2>
    friend
    constexpr
    unsigned
    popcount(Z<Unsigned, N2, P2> const& x) noexcept;

    template <unsigned N2>
    requires(N2 <= Nlimit)
    friend
    constexpr
    unsigned
    detail::divu10(Z<Unsigned, N2, Wrap>& u) noexcept;

    template <SignTag S1, unsigned N1, Policy P1>
    requires (N1 <= Nlimit)
    friend
    std::string
    to_string(Z<S1, N1, P1> const& x);

    // Precondition:  0 <= n < N
    constexpr Z left_shift(unsigned n) const noexcept
    {
        Z<S, N, P> r;
        r.rep_ = urep_t(rep_ << n);
        return r;
    }

    // Precondition:  0 <= n < N
    constexpr Z right_shift_unsigned(unsigned n) const noexcept
    {
        Z<S, N, P> r;
        r.rep_ = urep_t(rep_ >> n);
        return r;
    }

    // Precondition:  0 <= n < N
    constexpr Z right_shift_signed(unsigned n) const noexcept
    {
        Z<S, N, P> r;
        r.rep_ = urep_t(srep_t(rep_) >> n);
        return r;
    }

    constexpr
    bool
    high_bit() const noexcept
    {
        auto constexpr mask = urep_t(urep_t(1) << (N-1));
        return bool(rep_ & mask);
    }

    constexpr
    void
    zero_high_bit() noexcept
    {
        urep_t constexpr mask = urep_t(~urep_t(urep_t(1) << (N-1)));
        rep_ &= mask;
    }

    constexpr
    void
    set_high_bit() noexcept
    {
        urep_t constexpr mask = urep_t(urep_t(1) << (N-1));
        rep_ |= mask;
    }

    constexpr
    bool
    is_neg() const noexcept
    {
        if constexpr (sign{} == Unsigned{})
            return false;
        else
            return high_bit();
    }

    template <SignTag S2, unsigned N2, Policy P2>
    static
    constexpr
    auto
    sign_extend(Z<S2, N2, P2> const& y) noexcept
    {
        using X = Z<S2, N2, P2>;
        if (y.is_neg())
        {
            auto constexpr ones = ~X{};
            return ones;
        }
        auto constexpr zeroes = X{};
        return zeroes;
    }

    static_assert(ProperIntSize<size>);
};

template <SignTag S, unsigned N, Policy P>
class Z<S, N, P, false>
{
public:
    using sign = S;
    static constexpr auto size = N;
    using policy = P;

private:
    using utypes = std::tuple<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>;
    using stypes = std::tuple< std::int8_t,  std::int16_t,  std::int32_t,  std::int64_t>;
    template <unsigned N2>
        using urep_t = std::tuple_element_t<std::countr_zero(N2)-3, utypes>;
    template <unsigned N2>
        using srep_t = std::tuple_element_t<std::countr_zero(N2)-3, stypes>;
    template <SignTag S2, unsigned N2>
        using rep = std::conditional_t<S2{} == Signed{}, srep_t<N2>, urep_t<N2>>;

    using uhalf_t = Z<Unsigned, size/2, Wrap>;
    using shalf_t = Z<  Signed, size/2, Wrap>;

    uhalf_t lo_;
    uhalf_t hi_;

public:
    Z() = default;

    // Construct from integral type with bit with greater than uhalf_t
    // Implicit if lossless, otherwise explicit
    template <std::integral I>
        requires (size < 2*sizeof(I)*CHAR_BIT)
        constexpr
        explicit(detail::ExplicitFromI<Z, I>)
        Z(I i)
            noexcept(detail::NoexceptFromI<Z, I>)
            : lo_(i), hi_(i >> size/2)
            {detail::check(*this, i);}

    // Construct from integral type with bit with less than or equal to uhalf_t
    // Implicit if lossless, otherwise explicit
    template <std::integral I>
        requires (size >= 2*sizeof(I)*CHAR_BIT)
        constexpr
        explicit(detail::ExplicitFromI<Z, I>)
        Z(I i)
            noexcept(detail::NoexceptFromI<Z, I>)
            : lo_(i), hi_(-(i < 0))
            {detail::check(*this, i);}

    // Construction from other Z

    template <SignTag S2, unsigned N2, Policy P2>
        requires (N2 < size/2)
        explicit(detail::ExplicitFromZ<Z, Z<S2, N2, P2>>)
        constexpr
        Z(Z<S2, N2, P2> const& x)
            noexcept(detail::NoexceptFromZ<Z, Z<S2, N2, P2>>)
            : Z{Z<S2, size/2, policy>{x}} {}

    template <SignTag S2, unsigned N2, Policy P2>
        requires (N2 == size/2)
        explicit(detail::ExplicitFromZ<Z, Z<S2, N2, P2>>)
        constexpr
        Z(Z<S2, N2, P2> const& x)
            noexcept(detail::NoexceptFromZ<Z, Z<S2, N2, P2>>)
            : lo_{x}, hi_{uhalf_t::sign_extend(x)} {detail::check(*this, x);}

    template <SignTag S2, Policy P2>
        requires (sign{} != S2{} || P2{} != policy{})
        explicit(detail::ExplicitFromZ<Z, Z<S2, size, P2>>)
        constexpr
        Z(Z<S2, size, P2> const& x)
            noexcept(detail::NoexceptFromZ<Z, Z<S2, size, P2>>)
            : lo_{x.lo_}, hi_{x.hi_} {detail::check(*this, x);}

    template <SignTag S2, unsigned N2, Policy P2>
        requires (N2 == size*2)
        constexpr
        explicit(detail::ExplicitFromZ<Z, Z<S2, N2, P2>>)
        Z(Z<S2, N2, P2> const& x)
            noexcept(detail::NoexceptFromZ<Z, Z<S2, N2, P2>>)
            : lo_{x.lo_.lo_}, hi_{x.lo_.hi_} {detail::check(*this, x);}

    template <SignTag S2, unsigned N2, Policy P2>
        requires (N2 > size*2)
        constexpr
        explicit(detail::ExplicitFromZ<Z, Z<S2, N2, P2>>)
        Z(Z<S2, N2, P2> const& x)
            noexcept(detail::NoexceptFromZ<Z, Z<S2, N2, P2>>)
            : Z{Z<S2, size*2, policy>{x}} {}

    explicit constexpr Z(uhalf_t hi, uhalf_t lo) noexcept : lo_{lo}, hi_{hi} {} 

    explicit constexpr Z(std::string_view s);
    explicit operator std::string() const;

    constexpr explicit operator bool() const noexcept {return bool(hi_ | lo_);}

    template <std::integral I>
        requires (size > sizeof(I)*CHAR_BIT*2)
        constexpr explicit operator I() const noexcept
            {return I(Z<sign, size/2, policy>{*this});}

    template <std::integral I>
        requires (size == sizeof(I)*CHAR_BIT*2)
        constexpr explicit operator I() const noexcept {return I(lo_);}

    template <std::integral I>
        requires (size == sizeof(I)*CHAR_BIT)
        constexpr explicit operator I() const noexcept
            {return (I(urep_t<size/2>(hi_)) << size/2) | urep_t<size/2>(lo_);}

    template <std::integral I>
        requires (size < sizeof(I)*CHAR_BIT)
        constexpr explicit operator I() const noexcept
            {return I(Z<sign, 2*size, policy>{*this});}

    constexpr Z operator+() noexcept {return *this;}
    constexpr Z& operator++() noexcept(policy{} != Throw{})
        {*this += Z{1}; return *this;}
    constexpr Z operator++(int) noexcept(policy{} != Throw{})
        {auto t(*this); ++(*this); return t;}

    constexpr Z& operator--() noexcept(policy{} != Throw{})
        {*this -= Z{1}; return *this;}
    constexpr Z operator--(int) noexcept(policy{} != Throw{})
        {auto t(*this); --(*this); return t;}

    constexpr Z& operator+=(Z const& x) noexcept(policy{} != Throw{})
        {*this = *this + x; return *this;}
    constexpr Z& operator-=(Z const& x) noexcept(policy{} != Throw{})
        {*this = *this - x; return *this;}
    constexpr Z& operator*=(Z const& x) noexcept(policy{} != Throw{})
        {*this = *this * x; return *this;}
    constexpr Z& operator/=(Z const& x) noexcept(policy{} != Throw{})
        {*this = *this / x; return *this;}
    constexpr Z& operator%=(Z const& x) noexcept(policy{} != Throw{})
        {*this = *this % x; return *this;}
    constexpr Z& operator<<=(int n) noexcept(policy{} != Throw{})
        {*this = *this << n; return *this;}
    constexpr Z& operator<<=(Z const& n) noexcept(policy{} != Throw{})
        {*this = *this << n; return *this;}
    constexpr Z& operator>>=(int n) noexcept(policy{} != Throw{})
        {*this = *this >> n; return *this;}
    constexpr Z& operator>>=(Z const& n) noexcept(policy{} != Throw{})
        {*this = *this >> n; return *this;}
    constexpr Z& operator&=(Z const& y) noexcept(policy{} != Throw{})
        {*this = *this & y; return *this;}
    constexpr Z& operator^=(Z const& y) noexcept(policy{} != Throw{})
        {*this = *this ^ y; return *this;}
    constexpr Z& operator|=(Z const& y) noexcept(policy{} != Throw{})
        {*this = *this | y; return *this;}

    constexpr uhalf_t low() const noexcept {return lo_;}
    constexpr uhalf_t high() const noexcept {return hi_;}

private:
    template <SignTag S2, unsigned N2, Policy P2, bool>
    friend
    class Z;

    template <SignTag S1, unsigned N1, Policy P1>
    friend
    constexpr
    Z<S1, N1, P1>
    detail::zero_high_bit(Z<S1, N1, P1> x) noexcept;

    template <SignTag S1, unsigned N1, Policy P1>
    friend
    constexpr
    Z<S1, N1, P1>
    detail::set_high_bit(Z<S1, N1, P1> x) noexcept;

    template <SignTag S1, unsigned N1, Policy P1, std::integral I>
    friend
    constexpr
    void
    detail::check(Z<S1, N1, P1>& r, I i) noexcept (P1{} != Throw{});

    template <SignTag S1, unsigned N1, Policy P1, SignTag S2, unsigned N2, Policy P2>
    friend
    constexpr
    void
    detail::check(Z<S1, N1, P1>& r, Z<S2, N2, P2> const& x)
        noexcept (P1{} != Throw{});

    template <SignTag S2, unsigned N2, Policy P2>
    friend
    constexpr
    bool
    operator==(Z<S2, N2, P2> const& x, Z<S2, N2, P2> const& y) noexcept;

    template <SignTag S2, unsigned N2, Policy P2>
    friend
    constexpr
    bool
    operator<(Z<S2, N2, P2> const& x, Z<S2, N2, P2> const& y) noexcept;

    template <SignTag S2, unsigned N2, Policy P2>
    friend
    constexpr
    Z<S2, N2, P2>
    operator~(Z<S2, N2, P2> const& x) noexcept;

    template <SignTag S2, unsigned N2>
    friend
    constexpr
    Z<S2, N2, Wrap>
    operator<<(Z<S2, N2, Wrap> const& x, int n) noexcept;

    template <SignTag S2, unsigned N2>
    friend
    constexpr
    Z<S2, N2, Saturate>
    operator<<(Z<S2, N2, Saturate> const& x, int n) noexcept;

    template <SignTag S2, unsigned N2, ErrorCheckedPolicy P2>
    friend
    constexpr
    Z<S2, N2, P2>
    operator<<(Z<S2, N2, P2> const& x, int n) noexcept(P2{} == Terminate{});

    template <SignTag S2, unsigned N2>
    friend
    constexpr
    Z<S2, N2, Wrap>
    operator>>(Z<S2, N2, Wrap> const& x, int n) noexcept;

    template <SignTag S2, unsigned N2>
    friend
    constexpr
    Z<S2, N2, Saturate>
    operator>>(Z<S2, N2, Saturate> const& x, int n) noexcept;

    template <SignTag S2, unsigned N2, ErrorCheckedPolicy P2>
    friend
    constexpr
    Z<S2, N2, P2>
    operator>>(Z<S2, N2, P2> const& x, int n) noexcept(P2{} == Terminate{});

    template <SignTag S2, unsigned N2, Policy P2>
    friend
    constexpr
    Z<S2, N2, P2>
    operator&(Z<S2, N2, P2> const& x, Z<S2, N2, P2> const& y) noexcept;

    template <SignTag S2, unsigned N2, Policy P2>
    friend
    constexpr
    Z<S2, N2, P2>
    operator^(Z<S2, N2, P2> const& x, Z<S2, N2, P2> const& y) noexcept;

    template <SignTag S2, unsigned N2, Policy P2>
    friend
    constexpr
    Z<S2, N2, P2>
    operator|(Z<S2, N2, P2> const& x, Z<S2, N2, P2> const& y) noexcept;

    template <SignTag S2, unsigned N2>
    friend
    constexpr
    auto
    operator+(Z<S2, N2, Wrap> const& x, Z<S2, N2, Wrap> const& y) noexcept
        -> Z<S2, N2, Wrap>;

    template <SignTag S2, unsigned N2>
    friend
    constexpr
    auto
    operator+(Z<S2, N2, Saturate> const& x, Z<S2, N2, Saturate> const& y) noexcept
        -> Z<S2, N2, Saturate>;

    template <SignTag S2, unsigned N2, ErrorCheckedPolicy P2>
    friend
    constexpr
    auto
    operator+(Z<S2, N2, P2> const& x, Z<S2, N2, P2> const& y)
        noexcept(P2{} == Terminate{})
        -> Z<S2, N2, P2>;

    template <SignTag S2, unsigned N2>
    friend
    constexpr
    auto
    operator-(Z<S2, N2, Wrap> const& x, Z<S2, N2, Wrap> const& y) noexcept
        -> Z<S2, N2, Wrap>;

    template <SignTag S2, unsigned N2>
    friend
    constexpr
    auto
    operator-(Z<S2, N2, Saturate> const& x, Z<S2, N2, Saturate> const& y) noexcept
        -> Z<S2, N2, Saturate>;

    template <SignTag S2, unsigned N2>
    friend
    constexpr
    auto
    operator-(Z<S2, N2, Saturate> const& x) noexcept
        -> Z<S2, N2, Saturate>;

    template <SignTag S2, unsigned N2, ErrorCheckedPolicy P2>
    friend
    constexpr
    auto
    operator-(Z<S2, N2, P2> const& x, Z<S2, N2, P2> const& y)
        noexcept(P2{} == Terminate{})
        -> Z<S2, N2, P2>;

    template <SignTag S2, unsigned N2, ErrorCheckedPolicy P2>
    friend
    constexpr
    auto
    operator-(Z<S2, N2, P2> const& x) noexcept(P2{} == Terminate{})
        -> Z<S2, N2, P2>;

    template <SignTag S2, unsigned N2>
    friend
    constexpr
    auto
    operator*(Z<S2, N2, Wrap> const& x, Z<S2, N2, Wrap> const& y) noexcept
        -> Z<S2, N2, Wrap>;

    template <unsigned N2>
    friend
    constexpr
    Z<Unsigned, 2*N2, Wrap>
    detail::mul2(Z<Unsigned, N2, Wrap> const& x, Z<Unsigned, N2, Wrap> const& y)
        noexcept;

    template <unsigned N2>
    friend
    constexpr
    Z<Signed, 2*N2, Wrap>
    detail::mul2(Z<Signed, N2, Wrap> const& x, Z<Signed, N2, Wrap> const& y)
        noexcept;

    template <unsigned N2>
    friend
    constexpr
    auto
    operator/(Z<Unsigned, N2, Wrap> const& x,
              Z<Unsigned, N2, Wrap> const& y) noexcept
        -> Z<Unsigned, N2, Wrap>;

    template <unsigned N2>
    friend
    constexpr
    auto
    operator/(Z<Signed, N2, Wrap> const& x, Z<Signed, N2, Wrap> const& y) noexcept
        -> Z<Signed, N2, Wrap>;

    template <unsigned N2, Policy P2>
    friend
    constexpr
    unsigned
    countl_zero(Z<Unsigned, N2, P2> const& x) noexcept;

    template <unsigned N2, Policy P2>
    friend
    constexpr
    unsigned
    countr_zero(Z<Unsigned, N2, P2> const& x) noexcept;

    template <unsigned N2, Policy P2>
    friend
    constexpr
    unsigned
    popcount(Z<Unsigned, N2, P2> const& x) noexcept;

    template <unsigned N1>
    requires (N1 > Nlimit)
    friend
    void
    detail::push_digits(std::string& r, Z<Unsigned, N1, Wrap>& x);

    template <SignTag S1, unsigned N1, Policy P1>
    requires (N1 > Nlimit)
    friend
    std::string
    to_string(Z<S1, N1, P1> const& x);

    // Precondition:  0 <= n < N
    constexpr Z left_shift(unsigned n) const noexcept
    {
        if (n == 0)
            return *this;
        Z r;
        if (n < N/2)
        {
            r.lo_ = lo_.left_shift(n);
            r.hi_ = hi_.left_shift(n) | lo_.right_shift_unsigned(N/2 - n);
        }
        else
        {
            auto constexpr zeroes = uhalf_t{};
            r.lo_ = zeroes;
            r.hi_ = lo_.left_shift(n - N/2);
        }
        return r;
    }

    // Precondition:  0 <= n < N
    constexpr Z right_shift_unsigned(unsigned n) const noexcept
    {
        if (n == 0)
            return *this;
        Z r;
        if (n < N/2)
        {
            r.lo_ = hi_.left_shift(N/2 - n) | lo_.right_shift_unsigned(n);
            r.hi_ = hi_.right_shift_unsigned(n);
        }
        else
        {
            auto constexpr zeroes = uhalf_t{};
            r.lo_ = hi_.right_shift_unsigned(n - N/2);
            r.hi_ = zeroes;
        }
        return r;
    }

    // Precondition:  0 <= n < N
    constexpr Z right_shift_signed(unsigned n) const noexcept
    {
        if (n == 0)
            return *this;
        Z r;
        if (n < N/2)
        {
            r.lo_ = hi_.left_shift(N/2 - n) | lo_.right_shift_unsigned(n);
            r.hi_ = hi_.right_shift_signed(n);
        }
        else
        {
            r.lo_ = hi_.right_shift_signed(n - N/2);
            auto constexpr ones = ~uhalf_t{};
            auto constexpr zeroes = uhalf_t{};
            r.hi_ = hi_.high_bit() ? ones : zeroes;
        }
        return r;
    }

    constexpr
    bool
    high_bit() const noexcept
    {
        return hi_.high_bit();
    }

    constexpr
    void
    zero_high_bit() noexcept
    {
        hi_.zero_high_bit();
    }

    constexpr
    void
    set_high_bit() noexcept
    {
        hi_.set_high_bit();
    }

    constexpr
    bool
    is_neg() const noexcept
    {
        if constexpr (sign{} == Unsigned{})
            return false;
        else
            return high_bit();
    }

    template <SignTag S2, unsigned N2, Policy P2>
    static
    constexpr
    auto
    sign_extend(Z<S2, N2, P2> const& y) noexcept
    {
        using X = Z<sign, size, policy>;
        if (y.is_neg())
        {
            auto constexpr ones = ~X{};
            return ones;
        }
        auto constexpr zeroes = X{};
        return zeroes;
    }

    static_assert(ProperIntSize<size>);
};

}  // namespace bbi

namespace std
{

template <unsigned N1, unsigned N2, bbi::Policy P>
struct common_type<bbi::Z<bbi::Unsigned, N1, P>, bbi::Z<bbi::Signed, N2, P>>
{
    using type = bbi::Z<bbi::Signed, std::max(2*N1, N2), P>;
};

template <unsigned N1, unsigned N2, bbi::Policy P>
struct common_type<bbi::Z<bbi::Signed, N1, P>, bbi::Z<bbi::Unsigned, N2, P>>
{
    using type = bbi::Z<bbi::Signed, std::max(N1, 2*N2), P>;
};

template <bbi::SignTag S, unsigned N, bbi::Policy P, bbi::detail::StandardInteger I>
struct common_type<bbi::Z<S, N, P>, I>
{
    using type = conditional_t<(S{} == bbi::Signed{}) == is_signed_v<I>
        , bbi::Z<S, max<size_t>(N, sizeof(I)*CHAR_BIT), P>
        , bbi::Z<bbi::Signed, S{} == bbi::Signed{}
                                     ? max<size_t>(N, 2*sizeof(I)*CHAR_BIT)
                                     : max<size_t>(2*N, sizeof(I)*CHAR_BIT)
        , P>>;
};

template <bbi::SignTag S, unsigned N, bbi::Policy P, bbi::detail::StandardInteger I>
struct common_type<I, bbi::Z<S, N, P>>
{
    using type = common_type_t<bbi::Z<S, N, P>, I>;
};

template <bbi::SignTag S, unsigned N, bbi::Policy P>
class numeric_limits<bbi::Z<S, N, P>>
{
    using type = bbi::Z<S, N, P>;

public:
    static constexpr bool is_specialized = true;

    static constexpr type min() noexcept
    {
        if constexpr (S{} == bbi::Signed{})
        {
            auto constexpr z = bbi::detail::set_high_bit(type{});
            return z;
        }
        else
        {
            auto constexpr z = type{};
            return z;
        }
    }

    static constexpr type max() noexcept
    {
        if constexpr (S{} == bbi::Signed{})
        {
            auto constexpr z = bbi::detail::zero_high_bit(~type{});
            return z;
        }
        else
        {
            auto constexpr z = ~type{};
            return z;
        }
    }

    static constexpr type lowest() noexcept {return min();}

    static constexpr bool is_signed = S{} == bbi::Signed{};
    static constexpr int  digits = static_cast<int>(N - is_signed);
    static constexpr int  digits10 = static_cast<int>((N - is_signed)*76573ULL/254370);
    static constexpr int  max_digits10 = 0;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr int  radix = 2;
    static constexpr type epsilon() noexcept {return type{};}
    static constexpr type round_error() noexcept {return type{};}

    static constexpr int  min_exponent   = 0;
    static constexpr int  min_exponent10 = 0;
    static constexpr int  max_exponent   = 0;
    static constexpr int  max_exponent10 = 0;

    static constexpr bool has_infinity      = false;
    static constexpr bool has_quiet_NaN     = false;
    static constexpr bool has_signaling_NaN = false;
    static constexpr bool has_denorm_loss   = false;
    static constexpr type infinity()      noexcept {return type{};}
    static constexpr type quiet_NaN()     noexcept {return type{};}
    static constexpr type signaling_NaN() noexcept {return type{};}
    static constexpr type denorm_min()    noexcept {return type{};}

    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = is_same_v<P, bbi::Wrap>;

    static constexpr bool traps           = false;
    static constexpr bool tinyness_before = false;
    static constexpr float_round_style round_style = round_toward_zero;
};

}  // namespace std

namespace bbi
{

namespace detail
{

template <SignTag S, unsigned N, ErrorCheckedPolicy P, std::integral I>
void
conversion_overflowed(Z<S, N, P> const&, I const& y) noexcept (P{} != Throw{})
{
    using R = Z<S, N, P>;
    auto const msg = to_string<R>() + '{' + to_string<I>() +
                     '{' + std::to_string(y) + "}} overflowed";
    if constexpr (P{} == Throw{})
        throw std::overflow_error(msg);
    else
    {
        std::cerr << msg << '\n';
        std::terminate();
    }
}

template <SignTag S, unsigned N, Policy P, std::integral I>
inline
constexpr
void
check(Z<S, N, P>& r, I i) noexcept (P{} != Throw{})
{
    if constexpr (P{} == Wrap{})
        return;
    if constexpr (P{} == Saturate{})
    {
        using R = Z<S, N, P>;
        if constexpr ((S{} == Signed{}) == std::is_signed_v<I>)
        {
            // R and I have the same sign
            if constexpr (std::numeric_limits<R>::digits >=
                          std::numeric_limits<I>::digits)
            {
                return;  // I can not overflow R
            }
            else
            {
                // I can hold a superset of the values of R
                auto constexpr Rm = std::numeric_limits<R>::min();
                auto constexpr RM = std::numeric_limits<R>::max();
                auto constexpr Im = I{Rm};
                auto constexpr IM = I{RM};
                if (i < Im)
                    r = Rm;
                else if (i > IM)
                    r = RM;
                return;
            }
        }
        else
        {
            // R and I have different sign
            if constexpr (S{} == Unsigned{})
            {
                // R is unsigned, I is signed
                if (i < 0)
                {
                    auto constexpr zero = R{};
                    r = zero;
                    return;
                }
            }
            if constexpr (std::numeric_limits<R>::digits >
                          std::numeric_limits<I>::digits)
            {
                return;  // I can not overflow R
            }
            else
            {
                // I can hold a superset of the values of R but i is non-negative
                auto constexpr RM = std::numeric_limits<R>::max();
                auto constexpr IM = I{RM};
                if (i > IM)
                    r = RM;
                return;
            }
        }
    }
    if constexpr (ErrorCheckedPolicy<P>)
    {
        using R = Z<S, N, P>;
        if constexpr ((S{} == Signed{}) == std::is_signed_v<I>)
        {
            // R and I have the same sign
            if constexpr (std::numeric_limits<R>::digits >=
                          std::numeric_limits<I>::digits)
            {
                return;  // I can not overflow R
            }
            else
            {
                // I can hold a superset of the values of R
                auto constexpr m = I{std::numeric_limits<R>::min()};
                auto constexpr M = I{std::numeric_limits<R>::max()};
                if (i < m || i > M)
                    conversion_overflowed(R{}, i);
                return;
            }
        }
        else
        {
            // R and I have different sign
            if constexpr (S{} == Unsigned{})
            {
                // R is unsigned, I is signed
                if (i < 0)
                    conversion_overflowed(R{}, i);
            }
            if constexpr (std::numeric_limits<R>::digits >
                          std::numeric_limits<I>::digits)
            {
                return;  // I can not overflow R
            }
            else
            {
                // I can hold a superset of the values of R but i is non-negative
                auto constexpr M = I{std::numeric_limits<R>::max()};
                if (i > M)
                    conversion_overflowed(R{}, i);
                return;
            }
        }
    }
}

template <SignTag S1, unsigned N1, ErrorCheckedPolicy P1,
          SignTag S2, unsigned N2, Policy P2>
void
conversion_overflowed(Z<S1, N1, P1> const&, Z<S2, N2, P2> const& y)
    noexcept (P1{} != Throw{})
{
    using R = Z<S1, N1, P1>;
    using Y = Z<S2, N2, P2>;
    auto const msg = to_string<R>() + '{' + to_string<Y>() +
                     '{' + to_string(y) + "}} overflowed";
    if constexpr (P1{} == Throw{})
        throw std::overflow_error(msg);
     else
    {
        std::cerr << msg << '\n';
        std::terminate();
    }
}

template <SignTag S1, unsigned N1, Policy P1, SignTag S2, unsigned N2, Policy P2>
inline
constexpr
void
check(Z<S1, N1, P1>& r, Z<S2, N2, P2> const& x) noexcept (P1{} != Throw{})
{
    if constexpr (P1{} == Wrap{})
        return;
    else if constexpr (P1{} == Saturate{})
    {
        using RW = Z<S1, N1, Wrap>;
        using XW = Z<S2, N2, Wrap>;
        if constexpr (S1{} == S2{})
        {
            // R and X have the same sign
            if constexpr (std::numeric_limits<RW>::digits >=
                          std::numeric_limits<XW>::digits)
            {
                return;  // X can not overflow R
            }
            else
            {
                // X can hold a superset of the values of R
                using R = Z<S1, N1, P1>;
                auto constexpr Rm = std::numeric_limits<R>::min();
                auto constexpr RM = std::numeric_limits<R>::max();
                auto constexpr XWm = XW{Rm};
                auto constexpr XWM = XW{RM};
                XW xw{x};
                if (xw < XWm)
                    r = Rm;
                else if (xw > XWM)
                    r = RM;
                return;
            }
        }
        else
        {
            // R and X have different sign
            if constexpr (S1{} == Unsigned{})
            {
                using X = Z<S2, N2, P2>;
                auto constexpr Xzero = X{};
                // R is unsigned, X is signed
                if (x < Xzero)
                {
                    using R = Z<S1, N1, P1>;
                    auto constexpr Rzero = R{};
                    r = Rzero;
                    return;
                }
            }
            if constexpr (std::numeric_limits<RW>::digits >
                          std::numeric_limits<XW>::digits)
            {
                return;  // X can not overflow R
            }
            else
            {
                // X can hold a superset of the values of R but x is non-negative
                using R = Z<S1, N1, P1>;
                auto constexpr RM = std::numeric_limits<R>::max();
                auto constexpr XWM = XW{RM};
                XW xw{x};
                if (xw > XWM)
                    r = RM;
                return;
            }
        }
    }
    else if constexpr (ErrorCheckedPolicy<P1>)
    {
        using R = Z<S1, N1, P1>;
        using RW = Z<S1, N1, Wrap>;
        using XW = Z<S2, N2, Wrap>;
        auto constexpr m = XW{std::numeric_limits<RW>::min()};
        auto constexpr M = XW{std::numeric_limits<RW>::max()};
        if constexpr (S1{} == S2{})
        {
            // R and X have the same sign
            if constexpr (std::numeric_limits<RW>::digits >=
                          std::numeric_limits<XW>::digits)
            {
                return;  // X can not overflow R
            }
            else
            {
                // X can hold a superset of the values of R
                using R = Z<S1, N1, P1>;
                XW xw{x};
                if (xw < m || xw > M)
                    conversion_overflowed(R{}, x);
                return;
            }
        }
        else
        {
            // R and X have different sign
            if constexpr (S1{} == Unsigned{})
            {
                // R is unsigned, X is signed
                if (x.is_neg())
                    conversion_overflowed(R{}, x);
            }
            if constexpr (std::numeric_limits<RW>::digits >
                          std::numeric_limits<XW>::digits)
            {
                return;  // X can not overflow R
            }
            else
            {
                // X can hold a superset of the values of R but x is non-negative
                XW xw{x};
                if (xw > M)
                    conversion_overflowed(R{}, x);
               return;
            }
        }
    }
}

}  // namespace detail

template <SignTag S, unsigned N, Policy P>
inline
constexpr
bool
operator==(Z<S, N, P> const& x, Z<S, N, P> const& y) noexcept
{
    if constexpr (N <= Nlimit)
        return x.rep_ == y.rep_;
    else
        return x.hi_ == y.hi_ && x.lo_ == y.lo_;
}

template <SignTag S1, unsigned N1, Policy P, SignTag S2, unsigned N2>
inline
constexpr
bool
operator==(Z<S1, N1, P> const& x, Z<S2, N2, P> const& y) noexcept
{
    using R = std::common_type_t<decltype(x), decltype(y)>;
    return R{x} == R{y};
}

template <SignTag S, unsigned N, Policy P, detail::StandardInteger I>
constexpr
inline
bool
operator==(Z<S, N, P> const& x, I const& y) noexcept
{
    using R = std::common_type_t<decltype(x), decltype(y)>;
    return R{x} == R{y};
}

template <SignTag S, unsigned N, Policy P, detail::StandardInteger I>
constexpr
inline
bool
operator==(I const& x,  Z<S, N, P> const&y) noexcept
{
    using R = std::common_type_t<decltype(x), decltype(y)>;
    return R{x} == R{y};
}

template <SignTag S1, unsigned N1, Policy P, SignTag S2, unsigned N2>
inline
constexpr
bool
operator!=(Z<S1, N1, P> const& x, Z<S2, N2, P> const& y) noexcept
{
    return !(x == y);
}

template <SignTag S, unsigned N, Policy P, detail::StandardInteger I>
constexpr
inline
bool
operator!=(Z<S, N, P> const& x, I const& y) noexcept
{
    using R = std::common_type_t<decltype(x), decltype(y)>;
    return R{x} != R{y};
}

template <SignTag S, unsigned N, Policy P, detail::StandardInteger I>
constexpr
inline
bool
operator!=(I const& x,  Z<S, N, P> const&y) noexcept
{
    using R = std::common_type_t<decltype(x), decltype(y)>;
    return R{x} != R{y};
}

template <SignTag S, unsigned N, Policy P>
inline
constexpr
bool
operator<(Z<S, N, P> const& x, Z<S, N, P> const& y) noexcept
{
    if constexpr (N <= Nlimit)
    {
        if constexpr (S{} == Signed{})
        {
            using rep = typename Z<Signed, N, P>::rep;
            return rep(x.rep_) < rep(y.rep_);
        }
        else
        {
            return x.rep_ < y.rep_;
        }
    }
    else
    {
        if constexpr (S{} == Signed{})
        {
            using shalf_t = typename Z<Signed, N, P>::shalf_t;
            return shalf_t(x.hi_) < shalf_t(y.hi_) ?
                       true :
                       x.hi_ == y.hi_ ?
                           x.lo_ < y.lo_ :
                           false;
        }
        else
        {
            return x.hi_ < y.hi_ ?
                       true :
                       x.hi_ == y.hi_ ?
                           x.lo_ < y.lo_ :
                           false;
        }
    }
}

template <SignTag S1, unsigned N1, Policy P, SignTag S2, unsigned N2>
inline
constexpr
bool
operator<(Z<S1, N1, P> const& x, Z<S2, N2, P> const& y) noexcept
{
    using R = std::common_type_t<decltype(x), decltype(y)>;
    return R{x} < R{y};
}

template <SignTag S, unsigned N, Policy P, detail::StandardInteger I>
constexpr
inline
bool
operator<(Z<S, N, P> const& x, I const& y) noexcept
{
    using R = std::common_type_t<decltype(x), decltype(y)>;
    return R{x} < R{y};
}

template <SignTag S, unsigned N, Policy P, detail::StandardInteger I>
constexpr
inline
bool
operator<(I const& x, Z<S, N, P> const& y) noexcept
{
    using R = std::common_type_t<decltype(x), decltype(y)>;
    return R{x} < R{y};
}

template <SignTag S1, unsigned N1, Policy P, SignTag S2, unsigned N2>
inline
constexpr
bool
operator>(Z<S1, N1, P> const& x, Z<S2, N2, P> const& y) noexcept
{
    return y < x;
}

template <SignTag S, unsigned N, Policy P, detail::StandardInteger I>
constexpr
inline
bool
operator>(Z<S, N, P> const& x, I const& y) noexcept
{
    return y < x;
}

template <SignTag S, unsigned N, Policy P, detail::StandardInteger I>
constexpr
inline
bool
operator>(I const& x, Z<S, N, P> const& y) noexcept
{
    return y < x;
}

template <SignTag S1, unsigned N1, Policy P, SignTag S2, unsigned N2>
inline
constexpr
bool
operator<=(Z<S1, N1, P> const& x, Z<S2, N2, P> const& y) noexcept
{
    return !(y < x);
}

template <SignTag S, unsigned N, Policy P, detail::StandardInteger I>
inline
constexpr
bool
operator<=(Z<S, N, P> const& x, I const& y) noexcept
{
    return !(y < x);
}

template <SignTag S, unsigned N, Policy P, detail::StandardInteger I>
inline
constexpr
bool
operator<=( I const& x, Z<S, N, P> const& y) noexcept
{
    return !(y < x);
}

template <SignTag S1, unsigned N1, Policy P, SignTag S2, unsigned N2>
inline
constexpr
bool
operator>=(Z<S1, N1, P> const& x, Z<S2, N2, P> const& y) noexcept
{
    return !(x < y);
}

template <SignTag S, unsigned N, Policy P, detail::StandardInteger I>
inline
constexpr
bool
operator>=(Z<S, N, P> const& x, I const& y) noexcept
{
    return !(x < y);
}

template <SignTag S, unsigned N, Policy P, detail::StandardInteger I>
inline
constexpr
bool
operator>=(I const& x, Z<S, N, P> const& y) noexcept
{
    return !(x < y);
}

template <SignTag S, unsigned N>
inline
constexpr
Z<S, N, Wrap>
operator<<(Z<S, N, Wrap> const& x, int n) noexcept
{
    constexpr unsigned mask = N-1;
    n &= mask;
    return x.left_shift(n);
}

template <SignTag S, unsigned N>
inline
constexpr
Z<S, N, Wrap>
operator>>(Z<S, N, Wrap> const& x, int n) noexcept
{
    constexpr unsigned mask = N-1;
    n &= mask;
    if constexpr (S{} == Signed{})
        return x.right_shift_signed(n);
    else
        return x.right_shift_unsigned(n);
}

template <SignTag S, unsigned N>
inline
constexpr
Z<S, N, Saturate>
operator<<(Z<S, N, Saturate> const& x, int n) noexcept
{
    using X = Z<S, N, Saturate>;
    if (n < 0)
    {
        if (n == std::numeric_limits<int>::min())
            ++n;
        return x >> -n;
    }
    if (static_cast<unsigned>(n) >= N)
        return X{};
    return x.left_shift(unsigned(n));
}

template <SignTag S, unsigned N>
inline
constexpr
Z<S, N, Saturate>
operator>>(Z<S, N, Saturate> const& x, int n) noexcept
{
    using X = Z<S, N, Saturate>;
    if (n < 0)
    {
        if (n == std::numeric_limits<int>::min())
            ++n;
        return x << -n;
    }
    if (static_cast<unsigned>(n) >= N)
    {
        if (x.is_neg())
        {
            auto constexpr ones = ~X{};
            return ones;
        }
        auto constexpr zeroes = X{};
        return zeroes;
    }
    if constexpr (S{} == Signed{})
        return x.right_shift_signed(unsigned(n));
    else
        return x.right_shift_unsigned(unsigned(n));
}

template <SignTag S, unsigned N, ErrorCheckedPolicy P>
inline
constexpr
Z<S, N, P>
operator<<(Z<S, N, P> const& x, int n) noexcept(P{} == Terminate{})
{
    using X = Z<S, N, P>;
    if (n < 0 || static_cast<unsigned>(n) >= N)
    {
        auto const msg = to_string<X>() + '{' + to_string(x) + "} << " +
                         std::to_string(n) + " overflowed";
        if constexpr (P{} == Throw{})
            throw std::overflow_error(msg);
        else
        {
            std::cerr << msg << '\n';
            std::terminate();
        }
    }
    return x.left_shift(unsigned(n));
}

template <SignTag S, unsigned N, ErrorCheckedPolicy P>
inline
constexpr
Z<S, N, P>
operator>>(Z<S, N, P> const& x, int n) noexcept(P{} == Terminate{})
{
    using X = Z<S, N, P>;
    if (n < 0 || static_cast<unsigned>(n) >= N)
    {
        auto const msg = to_string<X>() + '{' + to_string(x) + "} >> " +
                         std::to_string(n) + " overflowed";
        if constexpr (P{} == Throw{})
            throw std::overflow_error(msg);
        else
        {
            std::cerr << msg << '\n';
            std::terminate();
        }
    }
    if constexpr (S{} == Signed{})
        return x.right_shift_signed(unsigned(n));
    else
        return x.right_shift_unsigned(unsigned(n));
}

template <SignTag S, unsigned N, Policy P>
inline
constexpr
Z<S, N, P>
operator<<(Z<S, N, P> const& x, Z<S, N, P> const& y) noexcept
{
    return x << static_cast<int>(y);
}

template <SignTag S, unsigned N, Policy P>
inline
constexpr
Z<S, N, P>
operator>>(Z<S, N, P> const& x, Z<S, N, P> const& y) noexcept
{
    return x >> static_cast<int>(y);
}

template <SignTag S, unsigned N, Policy P>
inline
constexpr
Z<S, N, P>
operator~(Z<S, N, P> const& x) noexcept
{
    Z<S, N, P> r;
    if constexpr (N <= Nlimit)
    {
        using urep_t = typename Z<S, N, P>::urep_t;
        r.rep_ = urep_t(~x.rep_);
    }
    else
    {
        r.lo_ = ~x.lo_;
        r.hi_ = ~x.hi_;
    }
    return r;
}

template <SignTag S, unsigned N, Policy P>
inline
constexpr
Z<S, N, P>
operator&(Z<S, N, P> const& x, Z<S, N, P> const& y) noexcept
{
    Z<S, N, P> r;
    if constexpr (N <= Nlimit)
    {
        using urep_t = typename Z<S, N, P>::urep_t;
        r.rep_ = urep_t(x.rep_ & y.rep_);
    }
    else
    {
        r.lo_ = x.lo_ & y.lo_;
        r.hi_ = x.hi_ & y.hi_;
    }
    return r;
}

template <SignTag S, unsigned N, Policy P>
inline
constexpr
Z<S, N, P>
operator^(Z<S, N, P> const& x, Z<S, N, P> const& y) noexcept
{
    Z<S, N, P> r;
    if constexpr (N <= Nlimit)
    {
        using urep_t = typename Z<S, N, P>::urep_t;
        r.rep_ = urep_t(x.rep_ ^ y.rep_);
    }
    else
    {
        r.lo_ = x.lo_ ^ y.lo_;
        r.hi_ = x.hi_ ^ y.hi_;
    }
    return r;
}

template <SignTag S, unsigned N, Policy P>
inline
constexpr
Z<S, N, P>
operator|(Z<S, N, P> const& x, Z<S, N, P> const& y) noexcept
{
    Z<S, N, P> r;
    if constexpr (N <= Nlimit)
    {
        using urep_t = typename Z<S, N, P>::urep_t;
        r.rep_ = urep_t(x.rep_ | y.rep_);
    }
    else
    {
        r.lo_ = x.lo_ | y.lo_;
        r.hi_ = x.hi_ | y.hi_;
    }
    return r;
}

// + Wrap

template <SignTag S, unsigned N>
constexpr
inline
auto
operator+(Z<S, N, Wrap> const& x, Z<S, N, Wrap> const& y) noexcept
    -> Z<S, N, Wrap>
{
    using R = Z<S, N, Wrap>;
    if constexpr (N <= Nlimit)
    {
        using rep = typename R::rep;
        using U = typename R::urep_t;
        return R(rep(U(rep{x}) + U(rep{y})));
    }
    else
    {
        R r;
        r.lo_ = x.lo_ + y.lo_;
        auto c = ((x.lo_ & y.lo_) | ((x.lo_ | y.lo_) & ~r.lo_)) >> (N/2-1);
        r.hi_ = x.hi_ + y.hi_ + c;
        return r;
    }
}

// + Saturate

template <SignTag S, unsigned N>
constexpr
inline
auto
operator+(Z<S, N, Saturate> const& x, Z<S, N, Saturate> const& y) noexcept
    -> Z<S, N, Saturate>
{
    using R = Z<S, N, Saturate>;
    using RW = Z<Unsigned, N, Wrap>;
    if constexpr (S{} == Unsigned{})
    {
        auto const xw = RW{x};
        auto r = xw + RW{y};
        r |= RW{-(r < xw)};
        return R{r};
    }
    else  // S is Signed
    {
        RW xw{x};
        RW yw{y};
        auto r = xw + yw;
        RW constexpr M{std::numeric_limits<R>::max()};
        xw = RW{xw.high_bit()} + M;
        if (!((xw ^ yw) | ~(yw ^ r)).high_bit())
            r = xw;
        using SW = Z<Signed, N, Wrap>;
        return R{SW{r}};
    }
}

// + Throw / Terminate

template <SignTag S, unsigned N, ErrorCheckedPolicy P>
constexpr
inline
auto
operator+(Z<S, N, P> const& x, Z<S, N, P> const& y) noexcept(P{} == Terminate{})
    -> Z<S, N, P>
{
    using R = Z<S, N, P>;
    using RW = Z<Unsigned, N, Wrap>;
    if constexpr (S{} == Unsigned{})
    {
        auto const xw = RW{x};
        auto r = xw + RW{y};
        if (r < xw)
        {
            auto const msg = to_string<R>() + '{' + to_string(x) + "} + " +
                             to_string<R>() + '{' + to_string(y) + "} overflowed";
            if constexpr (P{} == Throw{})
                throw std::overflow_error(msg);
            else
            {
                std::cerr << msg << '\n';
                std::terminate();
            }
        }
        return R{r};
    }
    else  // S is Signed
    {
        RW xw{x};
        RW yw{y};
        auto r = xw + yw;
        RW constexpr M{std::numeric_limits<R>::max()};
        xw = RW{xw.high_bit()} + M;
        if (!((xw ^ yw) | ~(yw ^ r)).high_bit())
        {
            auto const msg = to_string<R>() + '{' + to_string(x) + "} + " +
                             to_string<R>() + '{' + to_string(y) + "} overflowed";
            if constexpr (P{} == Throw{})
                throw std::overflow_error(msg);
            else
            {
                std::cerr << msg << '\n';
                std::terminate();
            }
        }
        using SW = Z<Signed, N, Wrap>;
        return R{SW{r}};
    }
}

template <SignTag S1, unsigned N1, Policy P, SignTag S2, unsigned N2>
constexpr
inline
auto
operator+(Z<S1, N1, P> const& x, Z<S2, N2, P> const& y) noexcept(P{} != Throw{})
    -> std::common_type_t<decltype(x), decltype(y)>
{
    using X = decltype(x);
    using Y = decltype(y);
    using R = std::common_type_t<X, Y>;
    return R{x} + R{y};
}

template <SignTag S, unsigned N, Policy P, detail::StandardInteger I>
constexpr
inline
auto
operator+(Z<S, N, P> const& x, I const& i) noexcept(P{} != Throw{})
    -> std::common_type_t<decltype(x), decltype(i)>
{
    using X = decltype(x);
    using R = std::common_type_t<X, I>;
    return R{x} + R{i};
}

template <SignTag S, unsigned N, Policy P, detail::StandardInteger I>
constexpr
inline
auto
operator+(I const& i, Z<S, N, P> const& y) noexcept(P{} != Throw{})
    -> std::common_type_t<decltype(i), decltype(y)>
{
    return y + i;
}

// - Wrap

template <SignTag S2, unsigned N2>
constexpr
inline
auto
operator-(Z<S2, N2, Wrap> const& x, Z<S2, N2, Wrap> const& y) noexcept
    -> Z<S2, N2, Wrap>
{
    using R = Z<S2, N2, Wrap>;
    if constexpr (N2 <= Nlimit)
    {
        using rep = typename R::rep;
        using U = typename R::urep_t;
        return R(rep(U(x) - U(y)));
    }
    else
    {
        R r;
        r.lo_ = x.lo_ - y.lo_;
        auto b = ((~x.lo_ & y.lo_) | (~(x.lo_ ^ y.lo_) & r.lo_)) >> (N2/2-1);
        r.hi_ = x.hi_ - y.hi_ - b;
        return r;
    }
}

template <SignTag S, unsigned N>
constexpr
inline
auto
operator-(Z<S, N, Wrap> const& x) noexcept
    -> Z<S, N, Wrap>
{
    using R = Z<S, N, Wrap>;
    if constexpr (N <= Nlimit)
    {
        using rep = typename R::rep;
        using U = typename R::urep_t;
        return R(rep(-U(x)));
    }
    else
    {
        return R{} - x;
    }
}

// - Saturate

template <SignTag S, unsigned N>
constexpr
inline
auto
operator-(Z<S, N, Saturate> const& x, Z<S, N, Saturate> const& y) noexcept
    -> Z<S, N, Saturate>
{
    using R = Z<S, N, Saturate>;
    using RW = Z<Unsigned, N, Wrap>;
    if constexpr (S{} == Unsigned{})
    {
        auto const xw = RW{x};
        RW r = xw - RW{y};
        if (r > xw)
            r = RW{};
        return R{r};
    }
    else  // S is Signed
    {
        RW xw{x};
        RW yw{y};
        auto r = xw - yw;
        RW constexpr M{std::numeric_limits<R>::max()};
        xw = RW{xw.high_bit()} + M;
        if (((xw ^ yw) & (xw ^ r)).high_bit())
            r = xw;
        using SW = Z<Signed, N, Wrap>;
        return R{SW{r}};
    }
}

template <SignTag S, unsigned N>
constexpr
inline
auto
operator-(Z<S, N, Saturate> const& x) noexcept
    -> Z<S, N, Saturate>
{
    using R = Z<S, N, Saturate>;
    if constexpr (S{} == Unsigned{})
    {
        return R{};
    }
    else  // S is Signed
    {
        using RW = Z<Unsigned, N, Wrap>;
        RW xw{x};
        auto r = -xw;
        RW constexpr M{std::numeric_limits<R>::max()};
        if (xw.high_bit() && r.high_bit())
            r = M;
        using SW = Z<Signed, N, Wrap>;
        return R{SW{r}};
    }
}

// - Throw / Terminate

template <SignTag S, unsigned N, ErrorCheckedPolicy P>
constexpr
inline
auto
operator-(Z<S, N, P> const& x, Z<S, N, P> const& y) noexcept(P{} == Terminate{})
    -> Z<S, N, P>
{
    using R = Z<S, N, P>;
    using RW = Z<Unsigned, N, Wrap>;
    if constexpr (S{} == Unsigned{})
    {
        auto const xw = RW{x};
        auto r = xw - RW{y};
        if (r > xw)
        {
            auto const msg = to_string<R>() + '{' + to_string(x) + "} - " +
                             to_string<R>() + '{' + to_string(y) + "} overflowed";
            if constexpr (P{} == Throw{})
                throw std::overflow_error(msg);
            else
            {
                std::cerr << msg << '\n';
                std::terminate();
            }
        }
        return R{r};
    }
    else  // S is Signed
    {
        RW xw{x};
        RW yw{y};
        auto r = xw - yw;
        RW constexpr M{std::numeric_limits<R>::max()};
        xw = RW{xw.high_bit()} + M;
        if (((xw ^ yw) & (xw ^ r)).high_bit())
        {
            auto const msg = to_string<R>() + '{' + to_string(x) + "} - " +
                             to_string<R>() + '{' + to_string(y) + "} overflowed";
            if constexpr (P{} == Throw{})
                throw std::overflow_error(msg);
            else
            {
                std::cerr << msg << '\n';
                std::terminate();
            }
        }
        using SW = Z<Signed, N, Wrap>;
        return R{SW{r}};
    }
}

template <SignTag S, unsigned N, ErrorCheckedPolicy P>
constexpr
inline
auto
operator-(Z<S, N, P> const& x) noexcept(P{} == Terminate{})
    -> Z<S, N, P>
{
    using R = Z<S, N, P>;
    if constexpr (S{} == Unsigned{})
    {
        if (x)
        {
            auto const msg = '-' +
                             to_string<R>() + '{' + to_string(x) + "} overflowed";
            if constexpr (P{} == Throw{})
                throw std::overflow_error(msg);
            else
            {
                std::cerr << msg << '\n';
                std::terminate();
            }
        }
        return x;
    }
    else  // S is Signed
    {
        using RW = Z<Unsigned, N, Wrap>;
        RW xw{x};
        auto r = -xw;
        if (xw.high_bit() && r.high_bit())
        {
            auto const msg = '-' +
                             to_string<R>() + '{' + to_string(x) + "} overflowed";
            if constexpr (P{} == Throw{})
                throw std::overflow_error(msg);
            else
            {
                std::cerr << msg << '\n';
                std::terminate();
            }
        }
        using SW = Z<Signed, N, Wrap>;
        return R{SW{r}};
    }
}

template <SignTag S1, unsigned N1, Policy P, SignTag S2, unsigned N2>
constexpr
inline
auto
operator-(Z<S1, N1, P> const& x, Z<S2, N2, P> const& y) noexcept(P{} != Throw{})
    -> std::common_type_t<decltype(x), decltype(y)>
{
    using X = decltype(x);
    using Y = decltype(y);
    using R = std::common_type_t<X, Y>;
    auto z = R{x} - R{y};
    return z;
}

template <SignTag S, unsigned N, Policy P, detail::StandardInteger I>
constexpr
inline
auto
operator-(Z<S, N, P> const& x, I const& i) noexcept(P{} != Throw{})
    -> std::common_type_t<decltype(x), decltype(i)>
{
    using X = decltype(x);
    using R = std::common_type_t<X, I>;
    return R{x} - R{i};
}

template <SignTag S, unsigned N, Policy P, detail::StandardInteger I>
constexpr
inline
auto
operator-(I const& i, Z<S, N, P> const& y) noexcept(P{} != Throw{})
    -> std::common_type_t<decltype(i), decltype(y)>
{
    using Y = decltype(y);
    using R = std::common_type_t<I, Y>;
    return R{i} - R{y};
}

// abs

template <unsigned N, Policy P>
constexpr
inline
auto
abs(Z<Signed, N, P> const& x)
{
    auto y = x >> std::numeric_limits<Z<Signed, N, P>>::digits;
    return (x ^ y) - y;
}

// countl_zero

template <unsigned N, Policy P>
constexpr
inline
unsigned
countl_zero(Z<Unsigned, N, P> const& x) noexcept
{
    if constexpr (N <= Nlimit)
    {
        return std::countl_zero(x.rep_);
    }
    else
    {
        if (x.hi_)
            return countl_zero(x.hi_);
        return N/2 + countl_zero(x.lo_);
    }
}

// countl_one

template <unsigned N, Policy P>
constexpr
inline
unsigned
countl_one(Z<Unsigned, N, P> const& x) noexcept
{
    return countl_zero(~x);
}

// countr_zero

template <unsigned N, Policy P>
constexpr
inline
unsigned
countr_zero(Z<Unsigned, N, P> const& x) noexcept
{
    if constexpr (N <= Nlimit)
    {
        return std::countr_zero(x.rep_);
    }
    else
    {
        if (x.lo_)
            return countr_zero(x.lo_);
        return N/2 + countr_zero(x.hi_);
    }
}

// countr_one

template <unsigned N, Policy P>
constexpr
inline
unsigned
countr_one(Z<Unsigned, N, P> const& x) noexcept
{
    return countr_zero(~x);
}

// popcount

template <unsigned N, Policy P>
constexpr
inline
unsigned
popcount(Z<Unsigned, N, P> const& x) noexcept
{
    if constexpr (N <= Nlimit)
    {
        return std::popcount(x.rep_);
    }
    else
    {
        return popcount(x.hi_) + popcount(x.lo_);
    }
}

// * Wrap

namespace detail
{

template <unsigned N>
constexpr
inline
Z<Unsigned, 2*N, Wrap>
mul2(Z<Unsigned, N, Wrap> const& x, Z<Unsigned, N, Wrap> const& y) noexcept
{
    using R2 = Z<Unsigned, 2*N, Wrap>;
    if constexpr (N < Nlimit || N == 8)
    {
        using rep2 = std::tuple_element_t<std::countr_zero(2*N) - 3,
                                          typename Z<Unsigned, 8, Wrap>::utypes>;
        return R2{rep2{x.rep_}*rep2{y.rep_}};
    }
    else  // N >= Nlimit && N >= 16
    {
        using R = Z<Unsigned, N, Wrap>;
        using Rhalf = Z<Unsigned, N/2, Wrap>;

        R constexpr mask  = std::numeric_limits<Rhalf>::max();

        auto x0 = Rhalf(x & mask);
        auto x1 = Rhalf(x >> N/2);
        auto y0 = Rhalf(y & mask);
        auto y1 = Rhalf(y >> N/2);

        R t = mul2(x0, y0);
        auto r2 = R2{t & mask};
        R c = t >> N/2;

        t = mul2(x1, y0) + c;
        c = t >> N/2;
        t &= mask;

        t += mul2(x0, y1);
        c += t >> N/2;
        t &= mask;
        r2 |= t << N/2;

        r2 |= R2{mul2(x1, y1) + c} << N;
        return r2;
    }
}

template <unsigned N>
constexpr
inline
Z<Signed, 2*N, Wrap>
mul2(Z<Signed, N, Wrap> const& x, Z<Signed, N, Wrap> const& y) noexcept
{
    using R2 = Z<Signed, 2*N, Wrap>;
    if constexpr (N < Nlimit || N == 8)
    {
        using rep2 = std::tuple_element_t<std::countr_zero(2*N) - 3,
                                          typename Z<Signed, 8, Wrap>::stypes>;
        using rep = std::tuple_element_t<std::countr_zero(N) - 3,
                                          typename Z<Signed, 8, Wrap>::stypes>;
        return R2{rep2{rep(x.rep_)}*rep2{rep(y.rep_)}};
    }
    else  // N >= Nlimit && N >= 16
    {
        using RU = Z<Unsigned, N, Wrap>;
        auto r2u = mul2(RU{x}, RU{y});
        using R2U = decltype(r2u);
        if (x.is_neg())
            r2u -= R2U{y} << N;
        if (y.is_neg())
            r2u -= R2U{x} << N;
        return R2{r2u};
    }
}

}  // namespace detail

template <SignTag S, unsigned N>
constexpr
inline
auto
operator*(Z<S, N, Wrap> const& x, Z<S, N, Wrap> const& y) noexcept
    -> Z<S, N, Wrap>
{
    using R = Z<S, N, Wrap>;
    return R{detail::mul2(x, y)};
}

// * Saturate

template <SignTag S, unsigned N>
constexpr
inline
auto
operator*(Z<S, N, Saturate> const& x, Z<S, N, Saturate> const& y) noexcept
    -> Z<S, N, Saturate>
{
    using R = Z<S, N, Saturate>;
    using RW = Z<S, N, Wrap>;
    return R{detail::mul2(RW{x}, RW{y})};
}

// * Throw / Terminate

template <unsigned N, ErrorCheckedPolicy P>
constexpr
inline
auto
operator*(Z<Unsigned, N, P> const& x, Z<Unsigned, N, P> const& y)
    noexcept(P{} == Terminate{})
    -> Z<Unsigned, N, P>
{
    using R = Z<Unsigned, N, P>;
    using RW = Z<Unsigned, N, Wrap>;
    auto r2 = detail::mul2(RW{x}, RW{y});
    R hi{RW{r2 >> N}};
    R lo{RW{r2}};
    if (hi)
    {
        auto const msg = to_string<R>() + '{' + to_string(x) + "} * " +
                         to_string<R>() + '{' + to_string(y) + "} overflowed";
        if constexpr (P{} == Throw{})
            throw std::overflow_error(msg);
        else
        {
            std::cerr << msg << '\n';
            std::terminate();
        }
    }
    return lo;
}

template <unsigned N, ErrorCheckedPolicy P>
constexpr
inline
auto
operator*(Z<Signed, N, P> const& x, Z<Signed, N, P> const& y) noexcept(P{} == Terminate{})
    -> Z<Signed, N, P>
{
    using R = Z<Signed, N, P>;
    using RW = Z<Signed, N, Wrap>;
    auto r2 = detail::mul2(RW{x}, RW{y});
    R hi{RW{r2 >> N}};
    R lo{RW{r2}};
    if (hi != (lo >> (N-1)))
    {
        auto const msg = to_string<R>() + '{' + to_string(x) + ") * " +
                         to_string<R>() + '{' + to_string(y) + ") overflowed";
        if constexpr (P{} == Throw{})
            throw std::overflow_error(msg);
        else
        {
            std::cerr << msg << '\n';
            std::terminate();
        }
    }
    return lo;
}

template <SignTag S1, unsigned N1, Policy P, SignTag S2, unsigned N2>
constexpr
inline
auto
operator*(Z<S1, N1, P> const& x, Z<S2, N2, P> const& y) noexcept(P{} != Throw{})
    -> std::common_type_t<decltype(x), decltype(y)>
{
    using X = decltype(x);
    using Y = decltype(y);
    using R = std::common_type_t<X, Y>;
    auto z = R{x} * R{y};
    return z;
}

template <SignTag S, unsigned N, Policy P, detail::StandardInteger I>
constexpr
inline
auto
operator*(Z<S, N, P> const& x, I const& i) noexcept(P{} != Throw{})
    -> std::common_type_t<decltype(x), decltype(i)>
{
    using X = decltype(x);
    using R = std::common_type_t<X, I>;
    return R{x} * R{i};
}

template <SignTag S, unsigned N, Policy P, detail::StandardInteger I>
constexpr
inline
auto
operator*(I const& i, Z<S, N, P> const& y) noexcept(P{} != Throw{})
    -> std::common_type_t<decltype(i), decltype(y)>
{
    return y * i;
}

// /

template <unsigned N>
constexpr
inline
auto
operator/(Z<Unsigned, N, Wrap> const& x, Z<Unsigned, N, Wrap> const& y) noexcept
    -> Z<Unsigned, N, Wrap>
{
    using R = Z<Unsigned, N, Wrap>;
    if constexpr (N <= Nlimit)
    {
        return R{x.rep_ / y.rep_};
    }
    else
    {
        using R2 = Z<Unsigned, N/2, Wrap>;

        auto constexpr divlu = [](R x, R2 y) noexcept -> R2
        {
            constexpr R2 b = R2{1} << N/4;
            R2 constexpr mask = std::numeric_limits<R2>::max() >> N/4;
            auto s = countl_zero(y);
            y <<= s;
            R2 yn0 = y & mask;
            R2 yn1 = y >> N/4;

            x <<= s;
            R2 xn10(x);
            R2 xn32(x >> N/2);

            R2 xn1 = xn10 >> N/4;
            R2 xn0 = xn10 & mask;

            R2 q1 = xn32 / yn1;
            R2 rhat = xn32 - q1*yn1;

            auto adjust_hi = [&]()
                {
                    if (q1 >= b || q1*yn0 > (rhat << N/4) + xn1)
                    {
                        --q1;
                        rhat += yn1;
                        if (rhat < b)
                            return true;
                    }
                    return false;
                };
            do {} while (adjust_hi());

            R2 xn21 = (xn32 << N/4) + xn1 - q1*y;

            R2 q0 = xn21 / yn1;
            rhat = xn21 - q0*yn1;

            auto adjust_lo = [&]()
                {
                    if (q0 >= b || q0*yn0 > (rhat << N/4) + xn0)
                    {
                        --q0;
                        rhat += yn1;
                        if (rhat < b)
                            return true;
                    }
                    return false;
                };
            do {} while (adjust_lo());

            return (q1 << N/4) + q0;
        };
        if (!(y >> N/2))
        {
            if ((x >> N/2) < y)
                return divlu(x, R2(y));
            else
            {
                R2 x1(x >> N/2);
                R2 x0(x);
                R2 y0(y);
                R2 q1 = divlu(x1, y0);
                auto k = x1 - q1*y;
                R2 q0 = divlu((k << N/2) + x0, y0);
                return (R(q1) << N/2) | R(q0);
            }
        }

        auto n = countl_zero(y);
        R2 y1 = R2((y << n) >> N/2);
        R x1 = x >> 1;
        R q1 = divlu(x1, y1);
        R q = (q1 << n) >> (N/2-1);
        if (q)
            --q;
        if ((x - q*y) >= y)
            ++q;
        return q;
    }
}

template <unsigned N>
constexpr
inline
auto
operator/(Z<Unsigned, N, Saturate> const& x,
          Z<Unsigned, N, Saturate> const& y) noexcept
    -> Z<Unsigned, N, Saturate>
{
    using R  = Z<Unsigned, N, Saturate>;
    using RW = Z<Unsigned, N, Wrap>;

    if (!y)
        return std::numeric_limits<R>::max();
    return R{RW{x}/RW{y}};
}

template <unsigned N, ErrorCheckedPolicy P>
constexpr
inline
auto
operator/(Z<Unsigned, N, P> const& x, Z<Unsigned, N, P> const& y)
    noexcept(P{} == Terminate{})
    -> Z<Unsigned, N, P>
{
    using R  = Z<Unsigned, N, P>;
    using RW = Z<Unsigned, N, Wrap>;

    if (!y)
    {
        auto const msg = to_string<R>() + '{' + to_string(x) + "} / 0";
        if constexpr (P{} == Throw{})
            throw std::overflow_error(msg);
        else
        {
            std::cerr << msg << '\n';
            std::terminate();
        }
    }
    return R{RW{x}/RW{y}};
}

template <unsigned N>
constexpr
inline
auto
operator/(Z<Signed, N, Wrap> const& x, Z<Signed, N, Wrap> const& y) noexcept
    -> Z<Signed, N, Wrap>
{
    using R  = Z<Signed, N, Wrap>;

    if constexpr (N <= Nlimit)
    {
        using I = R::rep;
        return R{I(x.rep_)/I(y.rep_)};
    }
    else
    {
        using RU = Z<Unsigned, N, Wrap>;

        RU xu{abs(x)};
        RU yu{abs(y)};
        auto q = xu / yu;
        RU t{(x ^ y) >> (N-1)};
        return R{(q ^ t) - t};
    }
}

template <unsigned N>
constexpr
inline
auto
operator/(Z<Signed, N, Saturate> const& x, Z<Signed, N, Saturate> const& y) noexcept
    -> Z<Signed, N, Saturate>
{
    using R  = Z<Signed, N, Saturate>;
    using RS = Z<Signed, N, Wrap>;

    auto constexpr m = std::numeric_limits<R>::min();
    auto constexpr M = std::numeric_limits<R>::max();
    auto constexpr n_one = R{-1};
    if (!y)
    {
        if (x >= R{})
            return M;
        return m;
    }
    if (x == m && y == n_one)
        return M;
    return R{RS{x}/RS{y}};
}

template <unsigned N, ErrorCheckedPolicy P>
constexpr
inline
auto
operator/(Z<Signed, N, P> const& x, Z<Signed, N, P> const& y) noexcept(P{} == Terminate{})
    -> Z<Signed, N, P>
{
    using R  = Z<Signed, N, P>;
    using RS = Z<Signed, N, Wrap>;

    if (!y)
    {
        auto const msg = to_string<R>() + '{' + to_string(x) + "} / 0";
        if constexpr (P{} == Throw{})
            throw std::overflow_error(msg);
        else
        {
            std::cerr << msg << '\n';
            std::terminate();
        }
    }
    auto constexpr m = std::numeric_limits<R>::min();
    auto constexpr n_one = R{-1};
    if (x == m && y == n_one)
    {
        auto const msg = to_string<R>() + '{' + to_string(x) + "} / "
                       + to_string<R>() + '{' + to_string(y) + "} overflowed";
        if constexpr (P{} == Throw{})
            throw std::overflow_error(msg);
        else
        {
            std::cerr << msg << '\n';
            std::terminate();
        }
    }
    return R{RS{x}/RS{y}};
}

template <SignTag S1, unsigned N1, Policy P, SignTag S2, unsigned N2>
constexpr
inline
auto
operator/(Z<S1, N1, P> const& x, Z<S2, N2, P> const& y) noexcept(P{} != Throw{})
    -> std::common_type_t<decltype(x), decltype(y)>
{
    using X = decltype(x);
    using Y = decltype(y);
    using R = std::common_type_t<X, Y>;
    auto z = R{x} / R{y};
    return z;
}

template <SignTag S, unsigned N, Policy P, detail::StandardInteger I>
constexpr
inline
auto
operator/(Z<S, N, P> const& x, I const& i) noexcept(P{} != Throw{})
    -> std::common_type_t<decltype(x), decltype(i)>
{
    using X = decltype(x);
    using R = std::common_type_t<X, I>;
    return R{x} / R{i};
}

template <SignTag S, unsigned N, Policy P, detail::StandardInteger I>
constexpr
inline
auto
operator/(I const& i, Z<S, N, P> const& y) noexcept(P{} != Throw{})
    -> std::common_type_t<decltype(i), decltype(y)>
{
    using Y = decltype(y);
    using R = std::common_type_t<I, Y>;
    return R{i} / R{y};
}

template <SignTag S1, unsigned N1, Policy P, SignTag S2, unsigned N2>
constexpr
inline
auto
operator%(Z<S1, N1, P> const& x, Z<S2, N2, P> const& y) noexcept(P{} != Throw{})
    -> std::common_type_t<decltype(x), decltype(y)>
{
    using X = decltype(x);
    using Y = decltype(y);
    using R = std::common_type_t<X, Y>;
    R a{x};
    R b{y};
    auto z = a - a/b*b;
    return z;
}

template <SignTag S, unsigned N, Policy P, detail::StandardInteger I>
constexpr
inline
auto
operator%(Z<S, N, P> const& x, I const& i) noexcept(P{} != Throw{})
    -> std::common_type_t<decltype(x), decltype(i)>
{
    using X = decltype(x);
    using R = std::common_type_t<X, I>;
    return R{x} % R{i};
}

template <SignTag S, unsigned N, Policy P, detail::StandardInteger I>
constexpr
inline
auto
operator%(I const& i, Z<S, N, P> const& y) noexcept(P{} != Throw{})
    -> std::common_type_t<decltype(i), decltype(y)>
{
    using Y = decltype(y);
    using R = std::common_type_t<I, Y>;
    return R{i} % R{y};
}

namespace detail
{

template <SignTag S, unsigned N, Policy P>
constexpr
Z<S, N, P>
zero_high_bit(Z<S, N, P> x) noexcept
{
    x.zero_high_bit();
    return x;
}

template <SignTag S, unsigned N, Policy P>
constexpr
Z<S, N, P>
set_high_bit(Z<S, N, P> x) noexcept
{
    x.set_high_bit();
    return x;
}

template <unsigned N>
requires(N <= Nlimit)
constexpr
inline
unsigned
divu10(Z<Unsigned, N, Wrap>& u) noexcept
{
    unsigned r = u.rep_ % 10;
    u.rep_ /= 10;
    return r;
}

template <unsigned N>
requires(N > Nlimit)
constexpr
inline
unsigned
divu10(Z<Unsigned, N, Wrap>& u) noexcept
{
    auto q = (u >> 1) + (u >> 2);
    for (unsigned s = 4; s < N; s *= 2)
        q += q >> s;
    q >>= 3;
    // approximate remainder r >= 0
    auto r = static_cast<unsigned>(u - ((q << 3) + (q << 1)));
    auto c = static_cast<std::uint8_t>(r/10);
    u = q + c;
    r -= c * 10;
    // remainder r is in [0, 9]
    return r;
}

template <unsigned N>
requires (N <= Nlimit)
inline
void
push_digits(std::string& r, Z<Unsigned, N, Wrap>& x)
{
    while (x)
        r.push_back(static_cast<char>(divu10(x) + '0'));
}

template <unsigned N>
requires (N > Nlimit)
inline
void
push_digits(std::string& r, Z<Unsigned, N, Wrap>& x)
{
    while (x.hi_)
        r.push_back(static_cast<char>(divu10(x) + '0'));
    push_digits(r, x.lo_);
}

}  // namespace detail

template <SignTag S, unsigned N, Policy P>
requires (N <= Nlimit)
std::string
to_string(Z<S, N, P> const& x)
{
    std::string r;
    bool const neg = x.is_neg();
    using UW = Z<Unsigned, N, Wrap>;
    UW xu{neg ? -UW{x} : UW{x}};
    if (neg)
        r.push_back('-');
    do
    {
        r.push_back(static_cast<char>(detail::divu10(xu) + '0'));
    } while (xu);
    std::reverse(r.begin() + neg, r.end());
    return r;
}

template <SignTag S, unsigned N, Policy P>
requires (N > Nlimit)
std::string
to_string(Z<S, N, P> const& x)
{
    std::string r;
    bool const neg = x.is_neg();
    using UW = Z<Unsigned, N, Wrap>;
    UW xu{neg ? -UW{x} : UW{x}};
    if (neg)
        r.push_back('-');
    do
    {
        r.push_back(static_cast<char>(detail::divu10(xu) + '0'));
    } while (xu.hi_);
    detail::push_digits(r, xu.lo_);
    std::reverse(r.begin() + neg, r.end());
    return r;
}

template <SignTag S, unsigned N, Policy P>
std::string
to_string_hex(Z<S, N, P> const& x)
{
    Z<Unsigned, N, Wrap> xu{x};
    std::string r;
    Z<Unsigned, N, Wrap> mask{0xF};
    mask <<= N-4;
    for (auto i = 0; i < N/4; ++i)
    {
        auto j = unsigned((xu & mask) >> (N-4 - 4*i));
        auto c = j < 10 ? char(j + '0') : char(j - 10 + 'a');
        r.push_back(c);
        mask >>= 4;
    }
    return r;
}

template <SignTag S, unsigned N, Policy P>
inline
std::ostream&
operator<<(std::ostream& os, Z<S, N, P> const& x)
{
    return os << to_string(x);
}

template <SignTag S, unsigned N, Policy P>
std::istream&
operator>>(std::istream& is, Z<S, N, P>& x)
{
    using traits = std::istream::traits_type;
    bool neg = false;
    std::ws(is);
    auto ic = is.peek();
    if (traits::eq_int_type(ic, traits::eof()))
    {
        is.setstate(std::ios::failbit);
        return is;
    }
    auto c = static_cast<char>(traits::to_char_type(ic));
    if (('0' <= c && c <= '9') || c == '-' || c == '+')
    {
        if (c == '-' || c == '+')
        {
            (void)is.get();
            if (c == '-')
                neg = true;
            ic = is.peek();
            if (traits::eq_int_type(ic, traits::eof()))
            {
                is.setstate(std::ios::failbit);
                return is;
            }
            c = static_cast<char>(traits::to_char_type(ic));
            if (!('0' <= c && c <= '9'))
            {
                is.setstate(std::ios::failbit);
                return is;
            }
        }
        (void)is.get();
        x = Z<S, N, P>{c - '0'};
        while (true)
        {
            ic = is.peek();
            if (traits::eq_int_type(ic, traits::eof()))
                break;
            c = static_cast<char>(traits::to_char_type(ic));
            if (!('0' <= c && c <= '9'))
                break;
            (void)is.get();
            x = x*Z<S, N, P>{10} + Z<S, N, P>{c - '0'};
        }
        if (neg)
            x = -x;
    }
    else
    {
        is.setstate(std::ios::failbit);
        return is;
    }
    return is;
}

namespace detail
{

template <class Z>
constexpr
Z
from_string_view(std::string_view const s)
{
    auto const throw_error = [&s]()
    {
        throw std::runtime_error('\"' + std::string(s) + "\" is not a valid integer");
    };
    unsigned i = 0;
    if (i == s.size())
        throw_error();
    if constexpr (typename Z::sign{} == Signed{})
    {
        bool neg = false;
        auto c = s[i];
        if (c == '-' || c == '+')
        {
            ++i;
            if (c == '-')
                neg = true;
            if (i == s.size())
                throw_error();
            c = s[i];
        }
        if (!('0' <= c && c <= '9'))
            throw_error();
        ++i;
        Z z{-(c - '0')};
        while (true)
        {
            if (i == s.size())
                break;
            c = s[i];
            if (!('0' <= c && c <= '9'))
                break;
            ++i;
            z = z*Z{10} + Z{-(c - '0')};
        }
        if (!neg)
            z = -z;
        return z;
    }
    else
    {
        auto c = s[i];
        if (c == '+')
        {
            ++i;
            if (i == s.size())
                throw_error();
            c = s[i];
        }
        if (!('0' <= c && c <= '9'))
            throw_error();
        ++i;
        Z z{c - '0'};
        while (true)
        {
            if (i == s.size())
                break;
            c = s[i];
            if (!('0' <= c && c <= '9'))
                break;
            ++i;
            z = z*Z{10} + Z{c - '0'};
        }
        return z;
    }
}

}  // namespace detail

template <SignTag S, unsigned N, Policy P>
constexpr
Z<S, N, P, true>::Z(std::string_view s)
    : Z{detail::from_string_view<Z>(s)}
    {}

template <SignTag S, unsigned N, Policy P>
constexpr
Z<S, N, P, false>::Z(std::string_view s)
    : Z{detail::from_string_view<Z>(s)}
    {}

template <SignTag S, unsigned N, Policy P>
Z<S, N, P, true>::operator std::string() const
{
    return to_string(*this);
}

template <SignTag S, unsigned N, Policy P>
Z<S, N, P, false>::operator std::string() const
{
    return to_string(*this);
}

// Policy namespaces

namespace raw
{

using u8   = std::uint8_t;
using u16  = std::uint16_t;
using u32  = std::uint32_t;
using u64  = std::uint64_t;
#ifndef _MSVC_LANG
using u128 = __uint128_t;
#endif

using i8    = std::int8_t;
using i16   = std::int16_t;
using i32   = std::int32_t;
using i64   = std::int64_t;
#ifndef _MSVC_LANG
using i128  = __int128_t;
#endif

}  // namespace raw

namespace wrap
{

using u8    = Z<Unsigned,    8, Wrap>;
using u16   = Z<Unsigned,   16, Wrap>;
using u32   = Z<Unsigned,   32, Wrap>;
using u64   = Z<Unsigned,   64, Wrap>;
using u128  = Z<Unsigned,  128, Wrap>;
using u256  = Z<Unsigned,  256, Wrap>;
using u512  = Z<Unsigned,  512, Wrap>;
using u1024 = Z<Unsigned, 1024, Wrap>;
using u2048 = Z<Unsigned, 2048, Wrap>;
using u4096 = Z<Unsigned, 4096, Wrap>;

using i8    = Z<Signed,    8, Wrap>;
using i16   = Z<Signed,   16, Wrap>;
using i32   = Z<Signed,   32, Wrap>;
using i64   = Z<Signed,   64, Wrap>;
using i128  = Z<Signed,  128, Wrap>;
using i256  = Z<Signed,  256, Wrap>;
using i512  = Z<Signed,  512, Wrap>;
using i1024 = Z<Signed, 1024, Wrap>;
using i2048 = Z<Signed, 2048, Wrap>;
using i4096 = Z<Signed, 4096, Wrap>;

}  // namespace wrap

namespace sat
{

using u8    = Z<Unsigned,    8, Saturate>;
using u16   = Z<Unsigned,   16, Saturate>;
using u32   = Z<Unsigned,   32, Saturate>;
using u64   = Z<Unsigned,   64, Saturate>;
using u128  = Z<Unsigned,  128, Saturate>;
using u256  = Z<Unsigned,  256, Saturate>;
using u512  = Z<Unsigned,  512, Saturate>;
using u1024 = Z<Unsigned, 1024, Saturate>;
using u2048 = Z<Unsigned, 2048, Saturate>;
using u4096 = Z<Unsigned, 4096, Saturate>;

using i8    = Z<Signed,    8, Saturate>;
using i16   = Z<Signed,   16, Saturate>;
using i32   = Z<Signed,   32, Saturate>;
using i64   = Z<Signed,   64, Saturate>;
using i128  = Z<Signed,  128, Saturate>;
using i256  = Z<Signed,  256, Saturate>;
using i512  = Z<Signed,  512, Saturate>;
using i1024 = Z<Signed, 1024, Saturate>;
using i2048 = Z<Signed, 2048, Saturate>;
using i4096 = Z<Signed, 4096, Saturate>;

}  // namespace sat

namespace thrw
{

using u8    = Z<Unsigned,    8, Throw>;
using u16   = Z<Unsigned,   16, Throw>;
using u32   = Z<Unsigned,   32, Throw>;
using u64   = Z<Unsigned,   64, Throw>;
using u128  = Z<Unsigned,  128, Throw>;
using u256  = Z<Unsigned,  256, Throw>;
using u512  = Z<Unsigned,  512, Throw>;
using u1024 = Z<Unsigned, 1024, Throw>;
using u2048 = Z<Unsigned, 2048, Throw>;
using u4096 = Z<Unsigned, 4096, Throw>;

using i8    = Z<Signed,    8, Throw>;
using i16   = Z<Signed,   16, Throw>;
using i32   = Z<Signed,   32, Throw>;
using i64   = Z<Signed,   64, Throw>;
using i128  = Z<Signed,  128, Throw>;
using i256  = Z<Signed,  256, Throw>;
using i512  = Z<Signed,  512, Throw>;
using i1024 = Z<Signed, 1024, Throw>;
using i2048 = Z<Signed, 2048, Throw>;
using i4096 = Z<Signed, 4096, Throw>;

}  // namespace thrw

namespace term
{

using u8    = Z<Unsigned,    8, Terminate>;
using u16   = Z<Unsigned,   16, Terminate>;
using u32   = Z<Unsigned,   32, Terminate>;
using u64   = Z<Unsigned,   64, Terminate>;
using u128  = Z<Unsigned,  128, Terminate>;
using u256  = Z<Unsigned,  256, Terminate>;
using u512  = Z<Unsigned,  512, Terminate>;
using u1024 = Z<Unsigned, 1024, Terminate>;
using u2048 = Z<Unsigned, 2048, Terminate>;
using u4096 = Z<Unsigned, 4096, Terminate>;

using i8    = Z<Signed,    8, Terminate>;
using i16   = Z<Signed,   16, Terminate>;
using i32   = Z<Signed,   32, Terminate>;
using i64   = Z<Signed,   64, Terminate>;
using i128  = Z<Signed,  128, Terminate>;
using i256  = Z<Signed,  256, Terminate>;
using i512  = Z<Signed,  512, Terminate>;
using i1024 = Z<Signed, 1024, Terminate>;
using i2048 = Z<Signed, 2048, Terminate>;
using i4096 = Z<Signed, 4096, Terminate>;

}  // namespace term

template <class T>
struct div_t
{
    T quot;
    T rem;
};

template <class T>
auto
constexpr
operator==(div_t<T> const& x, div_t<T> const& y) noexcept
{
    return x.quot == y.quot && x.rem == y.rem;
}

template <class T>
auto
constexpr
operator!=(div_t<T> const& x, div_t<T> const& y) noexcept
{
    return !(x == y);
}

template <class T>
std::ostream&
operator<<(std::ostream& os, div_t<T> const& x)
{
    return os << '{' << x.quot << ", " << x.rem << '}';
}

template <unsigned N1, unsigned N2, Policy P>
inline
constexpr
auto
trunc_div(Z<Signed, N1, P> const& n, Z<Signed, N2, P> const& d) noexcept(P{} != Throw{})
{
    auto const q = n/d;
    using R = decltype(q);
    using RW = Z<Signed, R::size, Wrap>;
    return div_t{q, R{RW{n} - RW{q}*RW{d}}};
}

template <unsigned N1, unsigned N2, Policy P>
inline
constexpr
auto
floor_div(Z<Signed, N1, P> const& n, Z<Signed, N2, P> const& d) noexcept(P{} != Throw{})
{
    auto const np = n >= 0;
    auto const dp = d >= 0;
    if (np == dp || d == 0)
        return trunc_div(n, d);
    std::int8_t constexpr one{1};
    using R = decltype(n/d);
    using RW = Z<Signed, R::size, Wrap>;
    using R2W = Z<Signed, 2*R::size, Wrap>;
    R const q{np ? (R2W{n} - (R2W{d}+one)) / R2W{d} : (R2W{n} + (one-R2W{d})) / R2W{d}};
    return div_t{q, R{RW{n} - RW{q}*RW{d}}};
}

template <unsigned N1, unsigned N2, Policy P>
inline
constexpr
auto
ceil_div(Z<Signed, N1, P> const& n, Z<Signed, N2, P> const& d) noexcept(P{} != Throw{})
{
    auto const np = n >= 0;
    auto const dp = d >= 0;
    if (np != dp || d == 0)
        return trunc_div(n, d);
    std::int8_t constexpr one{1};
    using R = decltype(n/d);
    using RW = Z<Signed, R::size, Wrap>;
    using R2W = Z<Signed, 2*R::size, Wrap>;
    R const q{np ? (R2W{n} + (R2W{d}-one)) / R2W{d} : (R2W{n} + (R2W{d}+one)) / R2W{d}};
    return div_t{q, R{RW{n} - RW{q}*RW{d}}};
}

template <unsigned N1, unsigned N2, Policy P>
inline
constexpr
auto
euc_div(Z<Signed, N1, P> const& n, Z<Signed, N2, P> const& d) noexcept(P{} != Throw{})
{
    auto const np = n >= 0;
    auto const dp = d >= 0;
    if (np || d == 0)
        return trunc_div(n, d);
    std::int8_t constexpr one{1};
    using R = decltype(n/d);
    using RW = Z<Signed, R::size, Wrap>;
    using R2W = Z<Signed, 2*R::size, Wrap>;
    R const q{dp ? (R2W{n} + (one-R2W{d})) / R2W{d} : (R2W{n} + (R2W{d}+one)) / R2W{d}};
    return div_t{q, R{RW{n} - RW{q}*RW{d}}};
}

template <detail::StandardSignedInteger I1, detail::StandardSignedInteger I2>
inline
constexpr
auto
trunc_div(I1 const& n, I2 const& d) noexcept
{
    auto const q = n/d;
    using R = decltype(q);
    return div_t{q, R{n} - R{q}*R{d}};
}

template <detail::StandardSignedInteger I1, detail::StandardSignedInteger I2>
requires (sizeof(I1)*CHAR_BIT <= 32 && sizeof(I2)*CHAR_BIT <= 32)
inline
constexpr
auto
floor_div(I1 const& n, I2 const& d) noexcept
{
    auto const np = n >= 0;
    auto const dp = d >= 0;
    if (np == dp || d == 0)
        return trunc_div(n, d);
    std::int8_t constexpr one{1};
    using R = decltype(n/d);
    using RW = R;
    using R2W = std::int64_t;
    static_assert(sizeof(R2W) >= 2*sizeof(RW));
    R const q(np ? (R2W{n} - (R2W{d}+one)) / R2W{d} : (R2W{n} + (one-R2W{d})) / R2W{d});
    return div_t{q, R{RW{n} - RW{q}*RW{d}}};
}

inline
constexpr
auto
floor_div(std::int64_t const& n, std::int64_t const& d) noexcept
{
    auto const np = n >= 0;
    auto const dp = d >= 0;
    if (np == dp || d == 0)
        return trunc_div(n, d);
    std::int8_t constexpr one{1};
    using R = decltype(n/d);
    using RW = R;
    using R2W = Z<Signed, 128, Wrap>;
    R const q(np ? (R2W{n} - (R2W{d}+one)) / R2W{d} : (R2W{n} + (one-R2W{d})) / R2W{d});
    return div_t{q, R{RW{n} - RW{q}*RW{d}}};
}

template <detail::StandardSignedInteger I1, detail::StandardSignedInteger I2>
requires (sizeof(I1)*CHAR_BIT <= 32 && sizeof(I2)*CHAR_BIT <= 32)
inline
constexpr
auto
ceil_div(I1 const& n, I2 const& d) noexcept
{
    auto const np = n >= 0;
    auto const dp = d >= 0;
    if (np != dp || d == 0)
        return trunc_div(n, d);
    std::int8_t constexpr one{1};
    using R = decltype(n/d);
    using RW = R;
    using R2W = std::int64_t;
    static_assert(sizeof(R2W) >= 2*sizeof(RW));
    R const q(np ? (R2W{n} + (R2W{d}-one)) / R2W{d} : (R2W{n} + (R2W{d}+one)) / R2W{d});
    return div_t{q, R{RW{n} - RW{q}*RW{d}}};
}

inline
constexpr
auto
ceil_div(std::int64_t const& n, std::int64_t const& d) noexcept
{
    auto const np = n >= 0;
    auto const dp = d >= 0;
    if (np != dp || d == 0)
        return trunc_div(n, d);
    std::int8_t constexpr one{1};
    using R = decltype(n/d);
    using RW = R;
    using R2W = Z<Signed, 128, Wrap>;
    R const q(np ? (R2W{n} + (R2W{d}-one)) / R2W{d} : (R2W{n} + (R2W{d}+one)) / R2W{d});
    return div_t{q, R{RW{n} - RW{q}*RW{d}}};
}


template <detail::StandardSignedInteger I1, detail::StandardSignedInteger I2>
requires (sizeof(I1)*CHAR_BIT <= 32 && sizeof(I2)*CHAR_BIT <= 32)
inline
constexpr
auto
euc_div(I1 const& n, I2 const& d) noexcept
{
    auto const np = n >= 0;
    auto const dp = d >= 0;
    if (np || d == 0)
        return trunc_div(n, d);
    std::int8_t constexpr one{1};
    using R = decltype(n/d);
    using RW = R;
    using R2W = std::int64_t;
    static_assert(sizeof(R2W) >= 2*sizeof(RW));
    R const q(dp ? (R2W{n} + (one-R2W{d})) / R2W{d} : (R2W{n} + (R2W{d}+one)) / R2W{d});
    return div_t{q, R{RW{n} - RW{q}*RW{d}}};
}

inline
constexpr
auto
euc_div(std::int64_t const& n, std::int64_t const& d) noexcept
{
    auto const np = n >= 0;
    auto const dp = d >= 0;
    if (np || d == 0)
        return trunc_div(n, d);
    std::int8_t constexpr one{1};
    using R = decltype(n/d);
    using RW = R;
    using R2W = Z<Signed, 128, Wrap>;
    R const q(dp ? (R2W{n} + (one-R2W{d})) / R2W{d} : (R2W{n} + (R2W{d}+one)) / R2W{d});
    return div_t{q, R{RW{n} - RW{q}*RW{d}}};
}

template <class Num, unsigned N, Policy P>
constexpr
Num
power(Num const& f, Z<Unsigned, N, P> n)
{
    if (n == 0)
        return Num{1};
    if (n == 1)
        return f;
    auto r = power(f, n >> 1);
    r *= r;
    if (n & Z<Unsigned, N, P>{1})
        r *= f;
    return r;
}

template <unsigned N, Policy P>
constexpr
Z<Unsigned, N, P>
gcd(Z<Unsigned, N, P> x, Z<Unsigned, N, P> y) noexcept
{
    if (x == 0)
        return y;
    if (y == 0)
        return x;
    auto xd = countr_zero(x);
    auto yd = countr_zero(y);
    auto d = std::min(xd, yd);
    x >>= xd;
    y >>= yd;
    while (x != y)
    {
        if (x > y)
        {
            x -= y;
            x >>= countr_zero(x);
        }
        else
        {
            y -= x;
            y >>= countr_zero(y);
        }
    }
    return x << d;
}

template <SignTag S1, unsigned N1, Policy P, SignTag S2, unsigned N2>
requires (!(S1{} == Unsigned{} && S2{} == Unsigned{} && N1 == N2))
constexpr
auto
gcd(Z<S1, N1, P> x, Z<S2, N2, P> y) noexcept
{
    using Ux = Z<Unsigned, N1, Wrap>;
    using Uy = Z<Unsigned, N2, Wrap>;
    Ux ux{x};
    if constexpr (S1{} == Signed{})
    {
        if (x < 0)
            ux = -ux;
    }
    Uy uy{y};
    if constexpr (S2{} == Signed{})
    {
        if (y < 0)
            uy = -uy;
    }
    using U = std::common_type_t<Ux, Uy>;
    return Z<Unsigned, U::size, P>{gcd(U{ux}, U{uy})};
}

template <unsigned N, Policy P>
constexpr
Z<Unsigned, N, P>
lcm(Z<Unsigned, N, P> x, Z<Unsigned, N, P> y) noexcept
{
    if (x == 0)
        return x;
    if (y == 0)
        return y;
    return x/gcd(x, y) * y;
}

template <SignTag S1, unsigned N1, Policy P, SignTag S2, unsigned N2>
requires (!(S1{} == Unsigned{} && S2{} == Unsigned{} && N1 == N2))
constexpr
auto
lcm(Z<S1, N1, P> x, Z<S2, N2, P> y) noexcept
{
    using Ux = Z<Unsigned, N1, Wrap>;
    using Uy = Z<Unsigned, N2, Wrap>;
    Ux ux{x};
    if constexpr (S1{} == Signed{})
    {
        if (x < 0)
            ux = -ux;
    }
    Uy uy{y};
    if constexpr (S2{} == Signed{})
    {
        if (y < 0)
            uy = -uy;
    }
    using R = Z<Unsigned, std::common_type_t<Ux, Uy>::size, P>;
    return lcm(R{ux}, R{uy});
}

template <unsigned N, Policy P>
constexpr
Z<Unsigned, N, P>
fac(Z<Unsigned, N, P> x)
{
    auto constexpr one = Z<Unsigned, N, P>{1};
    auto r = one;
    if (x > one)
    {
        for (; x != one; --x)
            r *= x;
    }
    return r;
}

template <unsigned N, Policy P>
constexpr
Z<Signed, N, P>
fac(Z<Signed, N, P> x)
{
    using Zu = Z<Unsigned, N, P>;
    using Zs = Z<Signed, N, P>;
    return Zs{fac(Zu{x})};
}

}  // namespace bbi

#endif  // BBI_H
