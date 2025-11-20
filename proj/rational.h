#ifndef RATIONAL_H
#define RATIONAL_H

#include "bbi.h"
#include <cassert>
#include <ostream>
#include <ratio>
#include <string>
#include <string_view>

namespace bbi
{

template <unsigned N> class rational;

namespace detail
{

template <class T>
struct isrational : std::false_type {};

template <unsigned N>
struct isrational<rational<N>> : std::true_type {};

template <class T>
concept isRational = isrational<T>::value;

template <class T>
concept OperatesWithRational = isZ<T> || StandardInteger<T>;

}  // namespace detail

}  // namespace bbi

namespace std
{

template <unsigned N1, unsigned N2>
struct common_type<bbi::rational<N1>, bbi::rational<N2>>
{
    using type = bbi::rational<std::max(N1, N2)>;
};

template <unsigned N, bbi::SignTag S, unsigned N2, bbi::Policy P>
struct common_type<bbi::rational<N>, bbi::Z<S, N2, P>>
{
    using Cz = std::common_type_t<typename bbi::rational<N>::value_type,
                            bbi::Z<S, N2, typename bbi::rational<N>::value_type::policy>>;
    using type = bbi::rational<Cz::size>;
};

template <unsigned N, bbi::detail::StandardInteger I>
struct common_type<bbi::rational<N>, I>
{
    using Cz = std::common_type_t<typename bbi::rational<N>::value_type, I>;
    using type = bbi::rational<Cz::size>;
};

template <unsigned N, bbi::detail::OperatesWithRational I>
struct common_type<I, bbi::rational<N>>
{
    using type = std::common_type_t<bbi::rational<N>, I>;
};

}  // namespace std

namespace bbi
{

template <unsigned N>
class rational
{
public:
    static constexpr auto size = N;
#ifndef NDEBUG
    using value_type = Z<Signed, size, Terminate>;
#else
    using value_type = Z<Signed, size, Wrap>;
#endif

private:
    value_type num_;
    value_type den_;

public:
    constexpr rational() noexcept;

    template <unsigned N2>
        requires(N2 <= N)
        constexpr
        rational(rational<N2> r) noexcept;

    template <unsigned N2>
        requires(N2 > N)
        constexpr
        explicit
        rational(rational<N2> r) noexcept;

    template <detail::OperatesWithRational I>
        explicit(!std::is_convertible_v<I, value_type>)
        constexpr rational(I i) noexcept;

    template <std::intmax_t Nr, std::intmax_t Dr>
        explicit(!(-std::numeric_limits<value_type>::max() <= Nr &&
                   Nr <= std::numeric_limits<value_type>::max() &&
                   Dr <= std::numeric_limits<value_type>::max()))
        constexpr rational(std::ratio<Nr, Dr> r) noexcept;

    explicit constexpr rational(std::string_view s);

    constexpr rational(value_type num, value_type den) noexcept;

    template <detail::OperatesWithRational I, detail::OperatesWithRational J>
        constexpr
        explicit(!(std::is_convertible_v<I, value_type> &&
                   std::is_convertible_v<J, value_type>))
        rational(I i, J j) noexcept;

    explicit operator std::string() const;

    constexpr value_type num() const noexcept {return num_;}
    constexpr value_type den() const noexcept {return den_;}

    constexpr rational operator+() const noexcept {return *this;}
    constexpr rational operator-() const noexcept;

    constexpr rational& operator++() {constexpr rational one{value_type{1}};
                                      *this += one; return *this;}
    constexpr rational operator++(int) {auto x = *this; ++(*this); return x;}

    constexpr rational& operator--() {constexpr rational one{value_type{1}};
                                      *this -= one; return *this;}
    constexpr rational operator--(int) {auto x = *this; --(*this); return x;}

    constexpr rational& operator+=(rational const& x) {*this = *this + x; return *this;}
    constexpr rational& operator-=(rational const& x) {*this = *this - x; return *this;}
    constexpr rational& operator*=(rational const& x) {*this = *this * x; return *this;}
    constexpr rational& operator/=(rational const& x) {*this = *this / x; return *this;}

    friend constexpr bool operator==(rational const& x, rational const& y) noexcept
        {
            if (x.num() == 0 && x.den() == 0)
                return false;
            if (y.num() == 0 && y.den() == 0)
                return false;
            return x.num() == y.num() && x.den() == y.den();
        }
    friend constexpr bool operator!=(rational const& x, rational const& y) noexcept
        {return !(x == y);}

    friend constexpr bool operator<(rational const& x, rational const& y) noexcept
        {
            if (x.den() == 0)
            {
                if (x.num() == 0)
                    return false;
                if (y.den() == 0)
                {
                    if (y.num() == 0)
                        return false;
                    return x.num() < y.num();
                }
                return x.num() < 0;
            }
            if (y.den() == 0)
                return y.num() > 0;
            if (x.num() < 0 != y.num() < 0)
                return x.num() < 0;
            using V2 = Z<Signed, 2*size, typename value_type::policy>;
            return V2{x.num()}*y.den() < V2{x.den()}*y.num();
        }
    friend constexpr bool operator>(rational const& x, rational const& y) noexcept
        {
            return y < x;
        }
    friend constexpr bool operator<=(rational const& x, rational const& y) noexcept
        {
            return !(y < x);
        }
    friend constexpr bool operator>=(rational const& x, rational const& y) noexcept
        {
            return !(x < y);
        }

    constexpr rational& reciprocal() noexcept;

    template <unsigned> friend class rational;
};

// rational class invariants:
// den_ >= 0 always.
// if den_ == 0, num_ will be one of 1, -1, 0, representing infinity, -infinity, or nan.
// -numeric_limits<value_type>::max() <= num_ && num_ <= numeric_limits<value_type>::max()
//   I.e. num_ is never numeric_limits<value_type>::min()
// If num_ == 0 then den_ == 1.
// If den_ != 0, gcd(num_, den_) == 1.
// No rational operation can cause the numerator or denominator to overflow.
//    Therefore there is no overthrow policy

template <unsigned N>
constexpr
rational<N>::rational() noexcept
    : num_{0}
    , den_{0}
{
}

template <unsigned N>
template <unsigned Ny>
requires (Ny <= N)
constexpr
rational<N>::rational(rational<Ny> r) noexcept
    : num_{r.num()}
    , den_{r.den()}
{
}

template <unsigned N>
template <unsigned Ny>
requires (Ny > N)
constexpr
rational<N>::rational(rational<Ny> r) noexcept
{
    if (r.num() == 0 || r.den() == 0)
    {   // r is +/-inf or nan, just copy it
        num_ = value_type{r.num_};
        den_ = value_type{r.den_};
        return;
    }

    using Z2 = typename rational<2*Ny>::value_type;
    auto constexpr M = std::numeric_limits<value_type>::max();

    if (Z2{2}*abs(r.num()) >= (++(Z2{2}*M))*r.den())
    {   // abs(r) >= M + 1/2, set *this to +/-inf
        num_ = r.num() < 0 ? value_type{-1} : value_type{1};
        den_ = value_type{0};
        return;
    }

    // r has a best approximation within the range [-M, M]
    // Use the convergents and semiconvergents of simple continued fractions
    // to find the best approximation of r that exists in rational<N>.

    // Just work with positives and negate the result if necessary
    bool neg = r.num() < 0;
    if (neg)
        r.num_ = -r.num_;

    // Will need original value of r for semiconvergents
    auto r0 = r;

    // initialize convergents
    value_type pm1{1};
    value_type qm1{0};
    value_type pm2{0};
    value_type qm2{1};
    num_ = pm1;
    den_ = qm1;

    while (true)
    {
        // Generate convergents
        auto const a = r.num() / r.den();
        if (a*pm1 > M - pm2 || a*qm1 > M - qm2)
        {
            // Find largest semiconvergent <= M
            if (a == 1)
                break;  // there are no semiconvergents
            auto k1 = pm1 != 0 ? (M - pm2) / pm1 : M;
            auto k2 = qm1 != 0 ? (M - qm2) / qm1 : M;
            auto k = std::min(k1, k2);

            // Get last semiconvergent with p <= M && q <= M
            auto pp1 = num_*k + pm2;
            auto qp1 = den_*k + qm2;

            // Is the last semiconvergent more accurate than the last convergent?
            if (abs(Z2{pp1} *r0.den() - Z2{qp1} *r0.num()) * den_ <
                abs(Z2{num_}*r0.den() - Z2{den_}*r0.num()) * qp1)
            {
                num_ = pp1;
                den_ = qp1;
                assert(gcd(num_, den_) == 1);
            }
            break;
        }
        
        // The convergent
        num_ = value_type{a*pm1 + pm2};
        den_ = value_type{a*qm1 + qm2};

        auto rem = r.num() - a * r.den();
        if (rem == 0)
            break;  // The convergent is exact

        // Iterate to next convergent
        r.num_ = r.den_;
        r.den_ = rem;
        pm2 = pm1;
        pm1 = num_;
        qm2 = qm1;
        qm1 = den_;
    }
    if (neg)
        num_ = -num_;
}

template <unsigned N>
template <detail::OperatesWithRational I>
constexpr
rational<N>::rational(I i) noexcept
    : rational{std::common_type_t<rational, I>{
        typename std::common_type_t<rational, I>::value_type{i},
        typename std::common_type_t<rational, I>::value_type{1}}}
{
}

template <unsigned N>
template <std::intmax_t Nr, std::intmax_t Dr>
constexpr
rational<N>::rational(std::ratio<Nr, Dr> r) noexcept
    : rational{rational<sizeof(std::intmax_t)*CHAR_BIT>{r.num, r.den}}
{
}

template <unsigned N>
constexpr
rational<N>::rational(std::string_view s)
{
    auto const throw_error = [&s](unsigned j)
    {
        throw std::runtime_error("\n\"" + std::string(s) + "\" is not a valid rational\n"
                               + std::string(j+1, ' ') + "^\n");
    };
    auto skipws = [&s](unsigned j)
        {
            while (j != s.size() && std::isspace(s[j]))
                ++j;
            return j;
        };
    if (s.empty())
        throw_error(0);
    using R = rational<N>;
    using V = typename R::value_type;
    using R2 = rational<2*N>;
    using V2 = typename R2::value_type;
    using signed_parse_type = Z<Signed, V2::size, Saturate>;
    signed_parse_type constexpr M{std::numeric_limits<V2>::max()};
    if (s[0] == '[')
    {
        using unsigned_parse_type = Z<Unsigned, V2::size, Saturate>;
        auto e = s.find(']', 1);
        if (e == std::string_view::npos)
            throw_error(s.size());
        unsigned i = skipws(1);
        signed_parse_type a{s.substr(i), i};
        bool neg = false;
        if (a < 0)
        {
            a = -a;
            neg = true;
        }
        signed_parse_type p{a};
        signed_parse_type q{1};

        signed_parse_type pm2{1};
        signed_parse_type pm1 = p;
        signed_parse_type qm2{0};
        signed_parse_type qm1 = q;
        i = skipws(i);
        if (i != e && s[i] == ';')
            i = skipws(i+1);

        while (i != e)
        {
            unsigned_parse_type au{s.substr(i), i};
            if (au == 0)
                throw_error(i-1);
            if (au > M)
                break;
            a = signed_parse_type{au};
            if (pm1 > (M - pm2)/a)
                break;
            if (qm1 > (M - qm2)/a)
                break;
            p = a*pm1 + pm2;
            q = a*qm1 + qm2;
            i = skipws(i);
            if (i == e)
                break;
            if (s[i] == ',')
                i = skipws(i+1);
            else
                throw_error(i);
            pm2 = pm1;
            pm1 = p;
            qm2 = qm1;
            qm1 = q;
        }

        *this = R{R2{V2{neg ? -p : p}, V2{q}}};
        return;
    }
    if (s == "nan")
    {
        num_ = V{0};
        den_ = V{0};
        return;
    }
    if (s == "inf")
    {
        num_ = V{1};
        den_ = V{0};
        return;
    }
    if (s == "-inf")
    {
        num_ = V{1};
        den_ = V{0};
        return;
    }
    auto i = s.find("/");
    signed_parse_type num{s.substr(0, i)};
    signed_parse_type den{1};
    if (i != std::string_view::npos)
        den = signed_parse_type{s.substr(i+1)};
    *this = R{R2{V2{num}, V2{den}}};
}

template <unsigned N>
constexpr
rational<N>::rational(value_type num, value_type den) noexcept
    : num_{num}
    , den_{den}
{
    auto g = gcd(num_, den_);
    if (g > 1)
    {
        num_ = value_type{num_ / g};
        den_ = value_type{den_ / g};
    }
    auto constexpr m = std::numeric_limits<value_type>::min();
    if (num_ == m || den_ == m)
    {
        using R2 = rational<2*size>;
        using Z2 = typename R2::value_type;
        R2 r2;
        r2.num_ = Z2{num_};
        r2.den_ = Z2{den_};
        if (r2.den_ < 0)
        {
            r2.num_ = -r2.num_;
            r2.den_ = -r2.den_;
        }
        *this = rational<size>(r2);
        return;
    }
    if (den_ < 0)
    {
        num_ = -num_;
        den_ = -den_;
    }
}

template <unsigned N>
template <detail::OperatesWithRational I, detail::OperatesWithRational J>
constexpr
rational<N>::rational(I i, J j) noexcept
    : rational{std::common_type_t<rational, I, J>{
        typename std::common_type_t<rational, I, J>::value_type{i},
        typename std::common_type_t<rational, I, J>::value_type{j}}}
{
}

template <unsigned N>
rational<N>::operator std::string() const
{
    if (den_ == 0)
    {
        if (num_ == 0)
            return "nan";
        assert(num_ == 1 || num_ == -1);
        if (num_ > 0)
            return "inf";
        return "-inf";
    }
    std::string s(num_);
    if (den_ != 1)
    {
        s += '/';
        s += std::string(den_);
    }
    return s;
}

template <unsigned N>
std::ostream&
operator<<(std::ostream& os, rational<N> const& r)
{
    return os << std::string(r);
}

template <unsigned N>
constexpr
rational<N>
rational<N>::operator-() const noexcept
{
    rational r = *this;
    r.num_ = -r.num_;
    return r;
}

template <unsigned Nx, unsigned Ny>
constexpr
auto
operator+(rational<Nx> const& x, rational<Ny> const& y) noexcept
{
    using R = std::common_type_t<rational<Nx>, rational<Ny>>;
    if (x.den() == 0)
    {
        if (x.num() == 0)
            return R{x};
        if (y.den() == 0)
        {
            if (y.num() == 0)
                return R{y};
            if ((x.num() < 0) != (y.num() < 0))
                return R{};
            return R{x};
        }
        return R{x};
    }
    if (y.den() == 0)
        return R{y};
    if (x.num() == 0)
        return R{y};
    if (y.num() == 0)
        return R{x};
    using R2 = rational<2*R::size>;
    using Z2 = typename R2::value_type;
    R2 r2{Z2{x.num()}*Z2{y.den()} + Z2{y.num()}*Z2{x.den()}, Z2{x.den()}*Z2{y.den()}};
    return R{r2};
}

template <unsigned Nx, detail::OperatesWithRational Y>
constexpr
auto
operator+(rational<Nx> const& x, Y y) noexcept
{
    using R = std::common_type_t<rational<Nx>, Y>;
    return R{x} + R{y};
}

template <unsigned Nx, detail::OperatesWithRational Y>
constexpr
auto
operator+(Y y, rational<Nx> const& x) noexcept
{
    using R = std::common_type_t<rational<Nx>, Y>;
    return R{y} + R{x};
}

template <unsigned Nx, unsigned Ny>
constexpr
auto
operator-(rational<Nx> const& x, rational<Ny> const& y) noexcept
{
    return x + -y;
}

template <unsigned Nx, detail::OperatesWithRational Y>
constexpr
auto
operator-(rational<Nx> const& x, Y y) noexcept
{
    using R = std::common_type_t<rational<Nx>, Y>;
    return R{x} - R{y};
}

template <unsigned Nx, detail::OperatesWithRational Y>
constexpr
auto
operator-(Y y, rational<Nx> const& x) noexcept
{
    using R = std::common_type_t<rational<Nx>, Y>;
    return R{y} - R{x};
}

template <unsigned Nx, unsigned Ny>
constexpr
auto
operator*(rational<Nx> const& x, rational<Ny> const& y) noexcept
{
    using R = std::common_type_t<rational<Nx>, rational<Ny>>;
    using R2 = rational<2*R::size>;
    using Z2 = typename R2::value_type;
    R2 r2{Z2{x.num()}*Z2{y.num()}, Z2{x.den()}*Z2{y.den()}};
    return R{r2};
}

template <unsigned Nx, detail::OperatesWithRational Y>
constexpr
auto
operator*(rational<Nx> const& x, Y y) noexcept
{
    using R = std::common_type_t<rational<Nx>, Y>;
    return R{x} * R{y};
}

template <unsigned Nx, detail::OperatesWithRational Y>
constexpr
auto
operator*(Y y, rational<Nx> const& x) noexcept
{
    using R = std::common_type_t<rational<Nx>, Y>;
    return R{y} * R{x};
}

template <unsigned Nx, unsigned Ny>
constexpr
auto
operator/(rational<Nx> const& x, rational<Ny> const& y) noexcept
{
    using R = std::common_type_t<rational<Nx>, rational<Ny>>;
    using R2 = rational<2*R::size>;
    using Z2 = typename R2::value_type;
    R2 r2{Z2{x.num()}*Z2{y.den()}, Z2{x.den()}*Z2{y.num()}};
    return R{r2};
}

template <unsigned Nx, detail::OperatesWithRational Y>
constexpr
auto
operator/(rational<Nx> const& x, Y y) noexcept
{
    using R = std::common_type_t<rational<Nx>, Y>;
    return R{x} / R{y};
}

template <unsigned Nx, detail::OperatesWithRational Y>
constexpr
auto
operator/(Y y, rational<Nx> const& x) noexcept
{
    using R = std::common_type_t<rational<Nx>, Y>;
    return R{y} / R{x};
}

template <unsigned Nx, unsigned Ny>
constexpr
auto
operator==(rational<Nx> const& x, rational<Ny> const& y) noexcept
{
    using C = std::common_type_t<rational<Nx>, rational<Ny>>;
    C const cx = x;
    C const cy = y;
    return cx.num() == cy.num() && cx.den() == cy.den();
}

template <unsigned Nx, detail::OperatesWithRational Y>
constexpr
auto
operator==(rational<Nx> const& x, Y y) noexcept
{
    using R = std::common_type_t<rational<Nx>, Y>;
    return R{x} == R{y};
}

template <unsigned Nx, detail::OperatesWithRational Y>
constexpr
auto
operator==(Y y, rational<Nx> const& x) noexcept
{
    using R = std::common_type_t<rational<Nx>, Y>;
    return R{y} == R{x};
}

template <unsigned Nx, unsigned Ny>
constexpr
auto
operator!=(rational<Nx> const& x, rational<Ny> const& y) noexcept
{
    return !(x == y);
}

template <unsigned Nx, detail::OperatesWithRational Y>
constexpr
auto
operator!=(rational<Nx> const& x, Y y) noexcept
{
    using R = std::common_type_t<rational<Nx>, Y>;
    return R{x} != R{y};
}

template <unsigned Nx, detail::OperatesWithRational Y>
constexpr
auto
operator!=(Y y, rational<Nx> const& x) noexcept
{
    using R = std::common_type_t<rational<Nx>, Y>;
    return R{y} != R{x};
}

template <unsigned Nx, unsigned Ny>
constexpr
auto
operator<(rational<Nx> const& x, rational<Ny> const& y) noexcept
{
    using C = rational<2*std::max(Nx, Ny)>;
    C const cx = x;
    C const cy = y;
    return cx.num()*cy.den() < cy.num()*cx.den();
}

template <unsigned Nx, detail::OperatesWithRational Y>
constexpr
auto
operator<(rational<Nx> const& x, Y y) noexcept
{
    using R = std::common_type_t<rational<Nx>, Y>;
    return R{x} < R{y};
}

template <unsigned Nx, detail::OperatesWithRational Y>
constexpr
auto
operator<(Y y, rational<Nx> const& x) noexcept
{
    using R = std::common_type_t<rational<Nx>, Y>;
    return R{y} < R{x};
}

template <unsigned Nx, unsigned Ny>
constexpr
auto
operator>(rational<Nx> const& x, rational<Ny> const& y) noexcept
{
    return y < x;
}

template <unsigned Nx, detail::OperatesWithRational Y>
constexpr
auto
operator>(rational<Nx> const& x, Y y) noexcept
{
    using R = std::common_type_t<rational<Nx>, Y>;
    return R{x} > R{y};
}

template <unsigned Nx, detail::OperatesWithRational Y>
constexpr
auto
operator>(Y y, rational<Nx> const& x) noexcept
{
    using R = std::common_type_t<rational<Nx>, Y>;
    return R{y} > R{x};
}

template <unsigned Nx, unsigned Ny>
constexpr
auto
operator<=(rational<Nx> const& x, rational<Ny> const& y) noexcept
{
    return !(y < x);
}

template <unsigned Nx, detail::OperatesWithRational Y>
constexpr
auto
operator<=(rational<Nx> const& x, Y y) noexcept
{
    using R = std::common_type_t<rational<Nx>, Y>;
    return R{x} <= R{y};
}

template <unsigned Nx, detail::OperatesWithRational Y>
constexpr
auto
operator<=(Y y, rational<Nx> const& x) noexcept
{
    using R = std::common_type_t<rational<Nx>, Y>;
    return R{y} <= R{x};
}

template <unsigned Nx, unsigned Ny>
constexpr
auto
operator>=(rational<Nx> const& x, rational<Ny> const& y) noexcept
{
    return !(x < y);
}

template <unsigned Nx, detail::OperatesWithRational Y>
constexpr
auto
operator>=(rational<Nx> const& x, Y y) noexcept
{
    using R = std::common_type_t<rational<Nx>, Y>;
    return R{x} >= R{y};
}

template <unsigned Nx, detail::OperatesWithRational Y>
constexpr
auto
operator>=(Y y, rational<Nx> const& x) noexcept
{
    using R = std::common_type_t<rational<Nx>, Y>;
    return R{y} >= R{x};
}

template <unsigned N>
constexpr
rational<N>&
rational<N>::reciprocal() noexcept
{
    std::swap(num_, den_);
    if (den_ < value_type{0})
    {
        num_ = -num_;
        den_ = -den_;
    }
    return *this;
}

template <unsigned N>
constexpr
rational<N>
reciprocal(rational<N> x) noexcept
{
    return x.reciprocal();
}

template <unsigned N>
constexpr
rational<N>
abs(rational<N> x)
{
    if (x.num() < 0)
        return -x;
    return x;
}

template <class Num, unsigned N, Policy P>
constexpr
Num
power(Num const& x, Z<Signed, N, P> n)
{
    if (n < 0)
        return Num{1}/power(x, -Z<Unsigned, N, Wrap>{n});
    return power(x, Z<Unsigned, N, Wrap>{n});
}

template <unsigned N, unsigned N2, Policy P>
constexpr
rational<N>
root(rational<N> f, Z<Unsigned, N2, P> du)
{
    using R = rational<N>;

    if (f == R{1} || du == 1)
        return f;
    if (du == 0)
    {
        if (f == -1)
            return R{1};
        if (abs(f) < 1)
            return R{0};
        return R{1, 0};
    }
    if (f < 0 && ((du & Z<Unsigned, N2, P>{1}) == 0))
        return R{0, 0};
    if (f == 0)
        return f;

    R rm1;
    R rm2;
    bool neg = f.num() < 0;
    if (neg)
        f = -f;
    bool recip = f.num() > f.den();
    if (recip)
        f.reciprocal();
    R r{1};
    if (du < 100'000)
    {
        // Quadratic least squares curve fit of f^(1/d) in the range [0, 1]
        bbi::term::i64 di{du};
        auto const D = ((6 * di + 11) * di + 6) * di + 1;
        auto const a0 = 3 * di * ((2 * di - 3) * di + 1);
        auto const a1 = 24 * di * (2 * di - 1);
        auto const a2 = -30 * (di - 1) * di;
        r = R{((a2 * f + a1) * f + a0) / D};
    }
    if (neg)
    {
        f = -f;
        r = -r;
    }
    do
    {
        rm2 = rm1;
        rm1 = r;
        r = (R(du - 1) * r + f / power(r, du - 1)) / R(du);
    } while (r.den() != 0 && r != rm1 && r != rm2);
    if (recip)
        r.reciprocal();
    return r;
}

template <unsigned N, detail::StandardUnsignedInteger U>
constexpr
rational<N>
root(rational<N> f, U du) 
{
    using Zu = Z<Unsigned, sizeof(U)*8, Wrap>;
    return root(f, Zu{du});
}

template <unsigned Nx, unsigned Ny>
constexpr
auto
power(rational<Nx> const& x, rational<Ny> const& y)
{
    using Zu = Z<Unsigned, Ny, Wrap>;
    return power(root(x, Zu{y.den()}), y.num());
}

template <unsigned N>
constexpr
auto
sqrt(rational<N> const& x)
{
    return root(x, 2u);
}

template <unsigned N>
constexpr
rational<N>
fac(rational<N> x)
{
    using R = rational<N>;
    using V = typename R::value_type;
    if (x.den() != V{1} || x.num() < V{0})
    {
        if (x.den() == V{0} && x.num() > V{0})
            return R{V{1}, V{0}};
        return R{V{0}, V{0}};
    }
    R r{V{1}};
    if (x.num() > V{1})
    {
        R constexpr inf{V{1}, V{0}};
        for (; x != R{1} && r != inf; --x)
            r *= x;
    }
    return r;
}

// Constants

namespace detail
{

template <unsigned N>
constexpr
rational<N>
smallarctan(bbi::rational<N> x) noexcept
{
    using R = rational<N>;
    using R2 = rational<2*R::size>;
    using Z2 = typename R2::value_type;
    R2 y = x;
    bool neg = true;
    Z2 j{3};
    while (true)
    {
        auto term = power(x, j) / j;
        if (term == 0)
            break;
        if (neg)
            y -= term;
        else
            y += term;
        neg = !neg;
        j += Z2{2};
    }
    return R{y};
}

}  // namespace detail

template <unsigned N>
constexpr
auto pi = []()
    {
        using R = rational<N>;
        using V = typename R::value_type;
        if constexpr (N == 8)
        {
            return R{V{22}, V{7}};
        }
        else if constexpr (N == 16)
        {
            return R{V{355}, V{113}};
        }
        else if constexpr (N == 32)
        {
            return R{V{1068966896}, V{340262731}};
        }
        else if constexpr (N == 64)
        {
            return R{V{2646693125139304345}, V{842468587426513207}};
        }
        else if constexpr (N == 128)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            return R{V{V2{7339140467331933381ull}, V2{8649395806047781895ull}},
                     V{V2{2336120966843279993ull}, V2{15050628954075209346ull}}};
        }
        else if constexpr (N == 256)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            return R{V{V2{V4{5990220675874303282ull}, V4{16093429834218994790ull}},
                       V2{V4{16273256030212878700ull}, V4{8567063727631140057ull}}},
                     V{V2{V4{1906746461553339108ull}, V4{18057623410632369104ull}},
                       V2{V4{12624995017122373895ull}, V4{16436642104513078625ull}}}};
        }
        else if constexpr (N == 512)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            return R{V{V2{V4{V8{8027671515209507518ull},  V8{1926030506614399867ull}},
                          V4{V8{2545998598498939448ull},  V8{12687426222155091959ull}}},
                       V2{V4{V8{12199471193193411538ull}, V8{14088506003209072021ull}},
                          V4{V8{2243115404150825199ull},  V8{5070687570371659079ull}}}},
                     V{V2{V4{V8{2555287206327196742ull},  V8{16028762100703359278ull}},
                          V4{V8{11019801650578434010ull}, V8{6423592340088360156ull}}},
                       V2{V4{V8{1454330049374977873ull},  V8{13207254730642606064ull}},
                          V4{V8{17542629854220895132ull}, V8{17556677310372916296ull}}}}};
        }
        else if constexpr (N == 1024)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            return R{V{V2{V4{V8{V16{6273392619242089402ull},  V16{15925604775069574237ull}},
                             V8{V16{10719369784461948170ull}, V16{15598086896995316660ull}}},
                          V4{V8{V16{12028580086121198270ull}, V16{13694744452836108631ull}},
                             V8{V16{4017437857747060667ull},  V16{6305784991208636103ull}}}},
                       V2{V4{V8{V16{15771113762849112626ull}, V16{12471459246481723387ull}},
                             V8{V16{12510958574524352175ull}, V16{15563194468483004483ull}}},
                          V4{V8{V16{14313218100476007574ull}, V16{5496248863437400460ull}},
                             V8{V16{5050635462307748338ull},  V16{17750216853567555893ull}}}}},
                     V{V2{V4{V8{V16{1996882890617181926ull},  V16{9920884172444366805ull}},
                             V8{V16{14414133348924370705ull}, V16{17259783334673703274ull}}},
                          V4{V8{V16{6201341561904941012ull},  V16{14601770419346407597ull}},
                             V8{V16{2012671642226985943ull},  V16{13120821370264283400ull}}}},
                       V2{V4{V8{V16{17746055710612509932ull}, V16{7728723341572740510ull}},
                             V8{V16{1818357198583771472ull},  V16{2884680102485926053ull}}},
                          V4{V8{V16{5306710188422352531ull},  V16{12615730101598242610ull}},
                             V8{V16{359143757567615320ull},   V16{15724317387603898994ull}}}}}};
        }
        else if constexpr (N == 2048)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            using V32 = Z<Unsigned, V::size/32, Wrap>;
            return R{V{V2{V4{V8{V16{V32{4565387827638453550ull},  V32{127266816627109173ull}},
                                V16{V32{13270515356132306826ull}, V32{3696778618362411437ull}}},
                             V8{V16{V32{2795968329070444466ull},  V32{6833003058538940242ull}},
                                V16{V32{15678356429648958470ull}, V32{6399631965093463145ull}}}},
                          V4{V8{V16{V32{9043233940395140970ull},  V32{3713551766055167618ull}},
                                V16{V32{9764293738421928421ull},  V32{2035157596444117344ull}}},
                             V8{V16{V32{2015014242855065310ull},  V32{557464979539140388ull}},
                                V16{V32{4683356941396204595ull},  V32{4202619477548837393ull}}}}},
                       V2{V4{V8{V16{V32{16864678346408795847ull}, V32{8154099339275901377ull}},
                                V16{V32{13505955164670314244ull}, V32{16709716009742385365ull}}},
                             V8{V16{V32{15250035802515253226ull}, V32{14038555323008258300ull}},
                                V16{V32{15524361043212445527ull}, V32{17201455983522848088ull}}}},
                          V4{V8{V16{V32{11323338734675768448ull}, V32{5231058461625789659ull}},
                                V16{V32{1505513042781733676ull},  V32{16799083951566863799ull}}},
                             V8{V16{V32{6951933872376269700ull},  V32{12655328718267664759ull}},
                                V16{V32{12542909517639822685ull}, V32{2767658945090520771ull}}}}}},
                     V{V2{V4{V8{V16{V32{1453208079800459493ull},  V32{7212905876425603518ull}},
                                V16{V32{10799779094438807196ull}, V32{2567087172582807546ull}}},
                             V8{V16{V32{4686943620943129691ull},  V32{1102296926979416111ull}},
                                V16{V32{6327930979087997453ull},  V32{5983178819569736400ull}}}},
                          V4{V8{V16{V32{8705688492580018293ull},  V32{9665532172362082331ull}},
                                V16{V32{16410021106517260206ull}, V32{17850904597215845572ull}}},
                             V8{V16{V32{18148914790450942540ull}, V32{16530334682802873591ull}},
                                V16{V32{13806805059574802930ull}, V32{4272300248696187592ull}}}}},
                       V2{V4{V8{V16{V32{9298937985118035720ull},  V32{10914348472565363467ull}},
                                V16{V32{14332329612830365160ull}, V32{17522153342298814229ull}}},
                             V8{V16{V32{2055451415084996541ull},  V32{2035709188739643420ull}},
                                V16{V32{8312661820347291349ull},  V32{16416628449815800415ull}}}},
                          V4{V8{V16{V32{5779820479349799881ull},  V32{13816907061532735268ull}},
                                V16{V32{12323066066457351719ull}, V32{12438025901146297646ull}}},
                             V8{V16{V32{6042502795049413820ull},  V32{16464600723579561562ull}},
                                V16{V32{11950796393152626712ull}, V32{13617469567587967684ull}}}}}}};
        }
        else if constexpr (N == 4096)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            using V32 = Z<Unsigned, V::size/32, Wrap>;
            using V64 = Z<Unsigned, V::size/64, Wrap>;
            return R{V{V2{V4{V8{V16{V32{V64{6226247255226833524ull},  V64{14320540525299008589ull}},
                                    V32{V64{11803361661120172475ull}, V64{14134618367853565723ull}}},
                                V16{V32{V64{11327664283047923122ull}, V64{15794886981954445611ull}},
                                    V32{V64{7271140862572938220ull},  V64{15507320925732725048ull}}}},
                             V8{V16{V32{V64{6121384617593458288ull},  V64{17803618728367588137ull}},
                                    V32{V64{14730916625249318575ull}, V64{2342382087670451619ull}}},
                                V16{V32{V64{11249810707390234529ull}, V64{12681995405217616537ull}},
                                    V32{V64{6550899218607780496ull},  V64{12692231221716964788ull}}}}},
                          V4{V8{V16{V32{V64{8184616892346020931ull},  V64{1941760814747373097ull}},
                                    V32{V64{3806176745127804957ull},  V64{17903419526227295260ull}}},
                                V16{V32{V64{11209961558418494557ull}, V64{9197618616861337867ull}},
                                    V32{V64{6372050878500340738ull},  V64{1271980642888686302ull}}}},
                             V8{V16{V32{V64{12230761675227212970ull}, V64{9036886544359193649ull}},
                                    V32{V64{17625970605183446835ull}, V64{14510933418230870074ull}}},
                                V16{V32{V64{4114964441676642678ull},  V64{853251635756274598ull}},
                                    V32{V64{12948157627597804084ull}, V64{16963174769736761198ull}}}}}},
                       V2{V4{V8{V16{V32{V64{9116486692664179783ull},  V64{15028513586941771506ull}},
                                    V32{V64{10485547193210369206ull}, V64{10668691551246080006ull}}},
                                V16{V32{V64{16795338287008742158ull}, V64{13300661161276505116ull}},
                                    V32{V64{15494057128579234957ull}, V64{13950564206970561592ull}}}},
                             V8{V16{V32{V64{1291584612433954525ull},  V64{10674252153736848126ull}},
                                    V32{V64{1076114315961506555ull},  V64{13793126347469191714ull}}},
                                V16{V32{V64{18228903896526894309ull}, V64{11619992761378181509ull}},
                                    V32{V64{14620108885260180544ull}, V64{16755716414045873420ull}}}}},
                          V4{V8{V16{V32{V64{11871089070526453901ull}, V64{9131001734795175423ull}},
                                    V32{V64{11090695073954766019ull}, V64{13459170494762251681ull}}},
                                V16{V32{V64{14724851335972371327ull}, V64{7780254674023812239ull}},
                                    V32{V64{5408298838432887370ull},  V64{8567747226084083727ull}}}},
                             V8{V16{V32{V64{16601866111257617436ull}, V64{7134977660871386254ull}},
                                    V32{V64{4381140545376506203ull},  V64{3566118577116198052ull}}},
                                V16{V32{V64{1110044840983688797ull},  V64{11589366026314233435ull}},
                                    V32{V64{11102027398876985252ull}, V64{4480321086057517938ull}}}}}}},
                     V{V2{V4{V8{V16{V32{V64{1981876055163392447ull},  V64{11268185398148524001ull}},
                                    V32{V64{3154916292759885220ull},  V64{11528779166956421952ull}}},
                                V16{V32{V64{16508022832569681559ull}, V64{9640084419636685691ull}},
                                    V32{V64{11754245818087266730ull}, V64{12098302662597871491ull}}}},
                             V8{V16{V32{V64{18340372152947381244ull}, V64{12768606219825592216ull}},
                                    V32{V64{6660448435786548790ull},  V64{12315819676017015314ull}}},
                                V16{V32{V64{12329679111813448921ull}, V64{16358150751099003146ull}},
                                    V32{V64{3387700956062992471ull},  V64{5634721515947117393ull}}}}},
                          V4{V8{V16{V32{V64{7610310012626532322ull},  V64{7643433646915606742ull}},
                                    V32{V64{3275442043100549449ull},  V64{10444548825287222633ull}}},
                                V16{V32{V64{16502772025578346693ull}, V64{8077521716272074188ull}},
                                    V32{V64{11631609554106187594ull}, V64{8875581627766249324ull}}}},
                             V8{V16{V32{V64{4749051511641831605ull},  V64{13339950010136079919ull}},
                                    V32{V64{9766702741045596676ull},  V64{6741870417871325411ull}}},
                                V16{V32{V64{14132398808055618951ull}, V64{17465624955124293692ull}},
                                    V32{V64{8232174499066112002ull},  V64{15101298927061341517ull}}}}}},
                       V2{V4{V8{V16{V32{V64{4612531248924794954ull},  V64{11464763124486415784ull}},
                                    V32{V64{10818750381530490874ull}, V64{10572351335443768487ull}}},
                                V16{V32{V64{14073601015997325696ull}, V64{2039220844632025825ull}},
                                    V32{V64{10403333909729125684ull}, V64{2840834887808745192ull}}}},
                             V8{V16{V32{V64{7105704227392173635ull},  V64{3977132024771362254ull}},
                                    V32{V64{5279296546823572994ull},  V64{17099791521967170143ull}}},
                                V16{V32{V64{14445041183078048350ull}, V64{1990299820180508647ull}},
                                    V32{V64{6075677898969944584ull},  V64{16641307830512824036ull}}}}},
                          V4{V8{V16{V32{V64{66579579082165916ull},    V64{5138581309714086923ull}},
                                    V32{V64{11605816096025059146ull}, V64{10990598026901461037ull}}},
                                V16{V32{V64{17842809303753786872ull}, V64{14226628419307614023ull}},
                                    V32{V64{7741625689146843807ull},  V64{5123069287396009753ull}}}},
                             V8{V16{V32{V64{274446036245443344ull},   V64{15805681923722762609ull}},
                                    V32{V64{15310075970504676823ull}, V64{2004777946556506071ull}}},
                                V16{V32{V64{14450638632147931345ull}, V64{9849084371385192256ull}},
                                    V32{V64{2164449044153152000ull},  V64{7605335076313702041ull}}}}}}}};
        }
        else
        {
            // compute from first principles if N >= 8192
            // Expect crippling slow compile and run times.
            return 16*smallarctan(R{1,5}) - 4*smallarctan(R{1,239});
        }
    }();

template <unsigned N>
constexpr
auto inv_pi = []()
    {
        return reciprocal(pi<N>);
    }();

template <unsigned N>
constexpr auto sqrt2 = []()
    {
        using R = rational<N>;
        using V = typename R::value_type;
        if constexpr (N == 8)
        {
            return R{V{99}, V{70}};
        }
        else if constexpr (N == 16)
        {
            return R{V{27720}, V{19601}};
        }
        else if constexpr (N == 32)
        {
            return R{V{1855077841}, V{1311738121}};
        }
        else if constexpr (N == 64)
        {
            return R{V{6882627592338442563}, V{4866752642924153522}};
        }
        else if constexpr (N == 128)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            return R{V{V2{7263297174055697708ull}, V2{12609675159645731416ull}},
                     V{V2{5135926685547871228ull}, V2{5608832401712713489ull}}};
        }
        else if constexpr (N == 256)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            return R{V{V2{V4{6904362574355789450ull}, V4{9272092543293334294ull}},
                       V2{V4{6271171421060415396ull}, V4{18299810076454686193ull}}},
                     V{V2{V4{4882121596097587172ull}, V4{16059636519875504046ull}},
                       V2{V4{297031346628575732ull},  V4{3448690639519024665ull}}}};
        }
        else if constexpr (N == 512)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            return R{V{V2{V4{V8{7309243842026551921ull},  V8{14949855734517606381ull}},
                          V4{V8{14454789123785695879ull}, V8{14825499998811231131ull}}},
                       V2{V4{V8{2734490013319936615ull},  V8{325511902330904194ull}},
                          V4{V8{9294594054942113145ull},  V8{9126215552394070564ull}}}},
                     V{V2{V4{V8{5168415886042988989ull},  V8{17209129396223312537ull}},
                          V4{V8{3118922381175259492ull},  V8{9055355962114431817ull}}},
                       V2{V4{V8{15270723466050662128ull}, V8{14814164222102713651ull}},
                          V4{V8{16606446917762226208ull}, V8{1414957494866590147ull}}}}};
        }
        else if constexpr (N == 1024)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            return R{V{V2{V4{V8{V16{6991991052893149559ull},  V16{18411222188032454929ull}},
                             V8{V16{2224311299505714626ull},  V16{13403440340821321208ull}}},
                          V4{V8{V16{3918730944939709926ull},  V16{4806140792951036172ull}},
                             V8{V16{15325203082518940905ull}, V16{11121200433628954762ull}}}},
                       V2{V4{V8{V16{7485365617884963914ull},  V16{11345075357271581711ull}},
                             V8{V16{14908965665714606569ull}, V16{8036031730078125541ull}}},
                          V4{V8{V16{15015163193343769855ull}, V16{9458472086306203273ull}},
                             V8{V16{14102312953995456278ull}, V16{15185818883187429353ull}}}}},
                     V{V2{V4{V8{V16{4944084287496414343ull},  V16{16083255055360826141ull}},
                             V8{V16{3867036146811635361ull},  V16{7092711778711410124ull}}},
                          V4{V8{V16{11661340687456006632ull}, V16{16540316764306295486ull}},
                             V8{V16{127896014130006977ull},   V16{4575943378627731377ull}}}},
                       V2{V4{V8{V16{17118117531856253747ull}, V16{16151037014335523608ull}},
                             V8{V16{17044093503722883294ull}, V16{12533455579375384383ull}}},
                          V4{V8{V16{4757497969130968609ull},  V16{11438431973822763413ull}},
                             V8{V16{6240428138136360009ull},  V16{7079875029868647037ull}}}}}};
        }
        else if constexpr (N == 2048)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            using V32 = Z<Unsigned, V::size/32, Wrap>;
            return R{V{V2{V4{V8{V16{V32{7495955484565439159ull},  V32{7166840898959246885ull}},
                                V16{V32{15171712319948635924ull}, V32{15540714914670600901ull}}},
                             V8{V16{V32{10256691379267991131ull}, V32{15175493837125525944ull}},
                                V16{V32{12097061923156827331ull}, V32{9098843706492771917ull}}}},
                          V4{V8{V16{V32{168929048121367152ull},   V32{15963750920518002449ull}},
                                V16{V32{912680109515401710ull},   V32{10110017301447173154ull}}},
                             V8{V16{V32{518605108494089161ull},   V32{4857689607651796421ull}},
                                V16{V32{12456354763144994854ull}, V32{2039502661596523509ull}}}}},
                       V2{V4{V8{V16{V32{6282449232614613817ull},  V32{17691850099365102950ull}},
                                V16{V32{1619632418206292257ull},  V32{4701550849156310693ull}}},
                             V8{V16{V32{9919340070363750639ull},  V32{12558045154565575728ull}},
                                V16{V32{6439200763887933995ull},  V32{12083289184078233777ull}}}},
                          V4{V8{V16{V32{3335603553426479448ull},  V32{9382696689478264808ull}},
                                V16{V32{11313767319626717370ull}, V32{14093606581755317285ull}}},
                             V8{V16{V32{12072102486150089153ull}, V32{10953044427077324890ull}},
                                V16{V32{6519397442199379164ull},  V32{9158690933945375508ull}}}}}},
                     V{V2{V4{V8{V16{V32{5300440954608714806ull},  V32{9429483298470126139ull}},
                                V16{V32{13240758099729513701ull}, V32{4877149919815678764ull}}},
                             V8{V16{V32{2506245159880503414ull},  V32{5032752366162335099ull}},
                                V16{V32{13664811454495752231ull}, V32{11869131896238850268ull}}}},
                          V4{V8{V16{V32{9529513904056707743ull},  V32{14564830984564174450ull}},
                                V16{V32{14400369921026997296ull}, V32{13773191668090803276ull}}},
                             V8{V16{V32{1147420890108641003ull},  V32{3105814013362258561ull}},
                                V16{V32{9091928030545985455ull},  V32{15916622349965073683ull}}}}},
                       V2{V4{V8{V16{V32{15541705999359968363ull}, V32{10279104580240344339ull}},
                                V16{V32{4946344809954294071ull},  V32{11040021789361307380ull}}},
                             V8{V16{V32{11642453980173292683ull}, V32{7215168055561212011ull}},
                                V16{V32{9512322639825761567ull},  V32{15089770418015462116ull}}}},
                          V4{V8{V16{V32{11257655732114741156ull}, V32{7102810842700445177ull}},
                                V16{V32{12064285707382676357ull}, V32{12698834807398445310ull}}},
                             V8{V16{V32{4835050835860058078ull},  V32{8670592258498807513ull}},
                                V16{V32{5150577910077657474ull},  V32{9045681264934994979ull}}}}}}};
        }
        else if constexpr (N == 4096)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            using V32 = Z<Unsigned, V::size/32, Wrap>;
            using V64 = Z<Unsigned, V::size/64, Wrap>;
            return R{V{V2{V4{V8{V16{V32{V64{7353768609418151860ull},  V64{765133518314623176ull}},
                                    V32{V64{11455719198900808459ull}, V64{17862897881211655386ull}}},
                                V16{V32{V64{10886564246291293420ull}, V64{5931176024241390086ull}},
                                    V32{V64{4457333518411500981ull},  V64{15165360621337526507ull}}}},
                             V8{V16{V32{V64{4922866909699328760ull},  V64{5204166275054957808ull}},
                                    V32{V64{4317856157031802844ull},  V64{5381090136301527595ull}}},
                                V16{V32{V64{14621061266153474626ull}, V64{18260532856374608836ull}},
                                    V32{V64{11812518546846221705ull}, V64{14248878550006456683ull}}}}},
                          V4{V8{V16{V32{V64{15323962627935118634ull}, V64{8281954961864506656ull}},
                                    V32{V64{11339542812211521335ull}, V64{17027952535715344456ull}}},
                                V16{V32{V64{257455917111878363ull},   V64{10154690882372645858ull}},
                                    V32{V64{9611861989213065641ull},  V64{7385650460706044584ull}}}},
                             V8{V16{V32{V64{13033961279292101326ull}, V64{10235755169864544493ull}},
                                    V32{V64{7228876029920833640ull},  V64{6279546118619244113ull}}},
                                V16{V32{V64{3881274352086410471ull},  V64{14534241158968968152ull}},
                                    V32{V64{12623185258442992201ull}, V64{9918876139622094705ull}}}}}},
                       V2{V4{V8{V16{V32{V64{18103676595830342745ull}, V64{6606799623656018439ull}},
                                    V32{V64{13316244325920553976ull}, V64{9508126095641638743ull}}},
                                V16{V32{V64{5707859330574843481ull},  V64{4287851480549135018ull}},
                                   V32{V64{10267977949713587245ull},  V64{5163624912911115661ull}}}},
                            V8{V16{V32{V64{9700127484103762431ull},   V64{4416096916467019192ull}},
                                   V32{V64{3285540274006160232ull},   V64{13338979083149305452ull}}},
                               V16{V32{V64{9731242556474889008ull},   V64{6221549843439177080ull}},
                                   V32{V64{17894540892578248773ull},  V64{11105065689293467611ull}}}}},
                         V4{V8{V16{V32{V64{10071599835409949560ull},  V64{8496991201483152965ull}},
                                   V32{V64{6528561385070016931ull},   V64{5858547694006778801ull}}},
                               V16{V32{V64{3092464432746439645ull},   V64{10453063553004686938ull}},
                                   V32{V64{5073164134243703931ull},   V64{18239998833825566354ull}}}},
                            V8{V16{V32{V64{7436815873554152163ull},   V64{2686123899091345682ull}},
                                   V32{V64{7189717500338909791ull},   V64{17584061087128719710ull}}},
                               V16{V32{V64{3006049530086110684ull},   V64{10459519825194322681ull}},
                                   V32{V64{2036694398362356980ull},   V64{4529184927689228553ull}}}}}}},
                    V{V2{V4{V8{V16{V32{V64{5199899650996342973ull},   V64{15249702188211011526ull}},
                                   V32{V64{16931483467106137042ull},  V64{651956326490567231ull}}},
                               V16{V32{V64{9016201469074261627ull},   V64{6065686516988726557ull}},
                                   V32{V64{12398212552490229661ull},  V64{9722917185372794899ull}}}},
                            V8{V16{V32{V64{274584679889606605ull},    V64{2426804241454371110ull}},
                                   V32{V64{3835079159531252519ull},   V64{13880711630367196846ull}}},
                               V16{V32{V64{17985941857465485457ull},  V64{14209266422850210352ull}},
                                   V32{V64{4502967485769902722ull},   V64{9090783340550485274ull}}}}},
                         V4{V8{V16{V32{V64{17105097982382758456ull},  V64{6228569296839337287ull}},
                                   V32{V64{14181938041971649217ull},  V64{1993367044738739981ull}}},
                               V16{V32{V64{13440297743690066078ull},  V64{12597676736240161818ull}},
                                   V32{V64{6258261931815709361ull},   V64{6932117177134371467ull}}}},
                            V8{V16{V32{V64{14607362007119903513ull},  V64{10772548034824505133ull}},
                                   V32{V64{11468250225508831370ull},  V64{592802717572311523ull}}},
                               V16{V32{V64{11101884157425785829ull},  V64{9823986476421620448ull}},
                                   V32{V64{7224839168294523211ull},   V64{15061031837555362977ull}}}}}},
                      V2{V4{V8{V16{V32{V64{2114826281188448690ull},   V64{18029288034792895691ull}},
                                   V32{V64{13566655182657320896ull},  V64{13643144059264592232ull}}},
                               V16{V32{V64{7150079107059964869ull},   V64{9958014628700440725ull}},
                                   V32{V64{10099714043762698847ull},  V64{5060902682921534642ull}}}},
                            V8{V16{V32{V64{7441807859100645788ull},   V64{10570747089249267223ull}},
                                   V32{V64{1814453934807456201ull},   V64{7351569892275941742ull}}},
                               V16{V32{V64{178235242965557764ull},    V64{8653186951306942475ull}},
                                   V32{V64{15640083749114300117ull},  V64{15807716368046057084ull}}}}},
                         V4{V8{V16{V32{V64{819521672056132502ull},    V64{17466103528238386040ull}},
                                   V32{V64{11966056554227677329ull},  V64{4951452739386316109ull}}},
                               V16{V32{V64{12299115363507893021ull},  V64{4160608864829586070ull}},
                                   V32{V64{3870687452087212412ull},   V64{11871787021594090506ull}}}},
                            V8{V16{V32{V64{10794080464254216887ull},  V64{13935129611332291791ull}},
                                   V32{V64{2809230813948307026ull},   V64{11797609049231997241ull}}},
                               V16{V32{V64{1951603424630405980ull},   V64{7186497366281883780ull}},
                                   V32{V64{6638853712871480928ull},   V64{4831125065464593741ull}}}}}}}};
        }
        else
        {
            //             -
            // sqrt2 = [1; 2]
        
            constexpr auto M = std::numeric_limits<V>::max();
        
            V p{1};
            V q{1};
        
            V pm2{1};
            V pm1 = p;
            V qm2{0};
            V qm1 = q;
        
            while (true)
            {
                if (pm1 > ((M - pm2) >> 1))
                    break;
                p = (pm1 << 1) + pm2;
                q = (qm1 << 1) + qm2;
        
                pm2 = pm1;
                pm1 = p;
                qm2 = qm1;
                qm1 = q;
            }
            return R{p, q};
        }
    }();

template <unsigned N>
constexpr auto sqrt3 = []()
    {
        using R = rational<N>;
        using V = typename R::value_type;
        if constexpr (N == 8)
        {
            return R{V{97}, V{56}};
        }
        else if constexpr (N == 8)
        {
            return R{V{18817}, V{10864}};
        }
        else if constexpr (N == 16)
        {
            return R{V{32592}, V{18817}};
        }
        else if constexpr (N == 32)
        {
            return R{V{1934726305}, V{1117014753}};
        }
        else if constexpr (N == 64)
        {
            return R{V{6483353477106532995}, V{3743165875258953026}};
        }
        else if constexpr (N == 128)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            return R{V{V2{5669385212177171088ull}, V2{4919760767747534234ull}},
                     V{V2{3273221078390173308ull}, V2{11543256130703104783ull}}};
        }
        else if constexpr (N == 256)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            return R{V{V2{V4{6035911086702719405ull}, V4{13047292410394959693ull}},
                       V2{V4{17718648109946987049ull}, V4{3284195530268376100ull}}},
                     V{V2{V4{3484834890712461626ull}, V4{4461846083623679207ull}},
                       V2{V4{16435230206456389638ull}, V4{16569780963687727431ull}}}};
        }
        else if constexpr (N == 512)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            return R{V{V2{V4{V8{8511044489723257381ull}, V8{9555092085930279215ull}},
                          V4{V8{98716648673750699ull}, V8{3585177852327971887ull}}},
                       V2{V4{V8{6787947737267458932ull}, V8{4686283453146574803ull}},
                          V4{V8{8950686331190393538ull}, V8{2720499729068200467ull}}}},
                     V{V2{V4{V8{4913853827226603702ull}, V8{16801176869627373090ull}},
                          V4{V8{2949476440848871440ull}, V8{17462271561081382887ull}}},
                       V2{V4{V8{17116379783923944534ull}, V8{13350752779512683911ull}},
                          V4{V8{6387089243117811475ull}, V8{12845098711465831098ull}}}}};
        }
        else if constexpr (N == 1024)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            return R{V{V2{V4{V8{V16{7152263587685392553ull},  V16{889647050022512134ull}},
                             V8{V16{13550522405781168847ull}, V16{17826291536236156023ull}}},
                          V4{V8{V16{3018962812128681485ull},  V16{15889590700853220192ull}},
                             V8{V16{4299730068289838654ull},  V16{6719782783350112448ull}}}},
                       V2{V4{V8{V16{9317072011579098406ull},  V16{11460422908671875726ull}},
                             V8{V16{17493978415999143238ull}, V16{16530730469977931065ull}}},
                          V4{V8{V16{2488823921312762768ull},  V16{3874279157600049000ull}},
                             V8{V16{11353207999269494555ull}, V16{5102998085527969251ull}}}}},
                     V{V2{V4{V8{V16{4129361307665319928ull},  V16{12152363858537429423ull}},
                             V8{V16{10016513551409338697ull}, V16{13058613117599459669ull}}},
                          V4{V8{V16{7080160532544364943ull},  V16{2656647371865090627ull}},
                             V8{V16{7219601017242291618ull},  V16{8374120717323555495ull}}}},
                       V2{V4{V8{V16{13096636587857613853ull}, V16{6809672465092068126ull}},
                             V8{V16{3731904631951393722ull},  V16{13297285843054620280ull}}},
                          V4{V8{V16{4063824197823145239ull},  V16{16788048858593068261ull}},
                             V8{V16{11906716219147619801ull}, V16{12417031689962845051ull}}}}}};
        }
        else if constexpr (N == 2048)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            using V32 = Z<Unsigned, V::size/32, Wrap>;
            return R{V{V2{V4{V8{V16{V32{7576280964972510363ull}, V32{13550579961792647078ull}},
                                V16{V32{17216997636543443674ull}, V32{3709555708190647690ull}}},
                             V8{V16{V32{11677744548878092216ull}, V32{4642665347779975194ull}},
                                V16{V32{9766036659737964316ull}, V32{7429764397980120839ull}}}},
                          V4{V8{V16{V32{17232949172706401212ull}, V32{12151923182807761786ull}},
                                V16{V32{1938690879028327726ull}, V32{13046098200439321110ull}}},
                             V8{V16{V32{13570937483418326255ull}, V32{8379711781203227234ull}},
                                V16{V32{17650088622385864317ull}, V32{16564665428223558722ull}}}}},
                       V2{V4{V8{V16{V32{14930851861415458550ull}, V32{10003641731632378927ull}},
                                V16{V32{16997325933890651721ull}, V32{8529090712294877379ull}}},
                             V8{V16{V32{3682321284643099112ull}, V32{9018347437719230634ull}},
                                V16{V32{12624108837162897993ull}, V32{2677296756605182765ull}}}},
                          V4{V8{V16{V32{9643227795860661614ull}, V32{6781891444978391676ull}},
                                V16{V32{14998033302451609055ull}, V32{11166019974772765629ull}}},
                             V8{V16{V32{4978715495942550149ull}, V32{11460506199086134467ull}},
                                V16{V32{12072709061632206437ull}, V32{367378066321389181ull}}}}}},
                     V{V2{V4{V8{V16{V32{4374167854583116506ull}, V32{4177641902819383426ull}},
                                V16{V32{11967375924462008709ull}, V32{11198679800686205133ull}}},
                             V8{V16{V32{15680714656807822156ull}, V32{15535594154486841280ull}},
                                V16{V32{18274652756805308848ull}, V32{13642316823451855637ull}}}},
                          V4{V8{V16{V32{12338390410090837547ull}, V32{577308185072971120ull}},
                                V16{V32{16150689208140612151ull}, V32{4569332977578531253ull}}},
                             V8{V16{V32{3599412952149988000ull}, V32{10335825254041447128ull}},
                                V16{V32{16386378925033892114ull}, V32{17857926223747536100ull}}}}},
                       V2{V4{V8{V16{V32{11314821146089000884ull}, V32{7767005961537887572ull}},
                                V16{V32{6851576984795127465ull}, V32{12076454423759703469ull}}},
                             V8{V16{V32{2789497332631542902ull}, V32{18340562374520249360ull}},
                                V16{V32{17760596753399520101ull}, V32{107493986150725391ull}}}},
                          V4{V8{V16{V32{8462950169898076341ull}, V32{5056796004879773925ull}},
                                V16{V32{14050686796779208513ull}, V32{11484473456007772466ull}}},
                             V8{V16{V32{11283886546337397714ull}, V32{10573490817024456065ull}},
                                V16{V32{10310810769599792746ull}, V32{16971567336621247579ull}}}}}}};
        }
        else if constexpr (N == 4096)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            using V32 = Z<Unsigned, V::size/32, Wrap>;
            using V64 = Z<Unsigned, V::size/64, Wrap>;
            return R{V{V2{V4{V8{V16{V32{V64{8501218783997972541ull},  V64{7835532639493961423ull}},
                                    V32{V64{11101598337351722807ull}, V64{3808853509471568390ull}}},
                                V16{V32{V64{4625161478920662994ull},  V64{16450599044920865679ull}},
                                    V32{V64{16064149520342937181ull}, V64{9515283853019225442ull}}}},
                             V8{V16{V32{V64{2533735714279697771ull},  V64{4539753300795441127ull}},
                                    V32{V64{11233701700064881283ull}, V64{8676534263339337676ull}}},
                                V16{V32{V64{7174430770850139341ull},  V64{7767083839491756075ull}},
                                    V32{V64{10658024787214226863ull}, V64{16493509802225540770ull}}}}},
                          V4{V8{V16{V32{V64{7206538228419744600ull},  V64{16059929570779374651ull}},
                                    V32{V64{15579104567819055910ull}, V64{9294097043833131835ull}}},
                                V16{V32{V64{1572363213130219594ull},  V64{12108616598002900222ull}},
                                    V32{V64{399609804863550244ull},   V64{10064740245277424016ull}}}},
                             V8{V16{V32{V64{8038782178759562524ull},  V64{13994475838667565574ull}},
                                    V32{V64{1697996986530231145ull},  V64{18199085964246631980ull}}},
                                V16{V32{V64{1609328118520109553ull},  V64{17653036356474622612ull}},
                                    V32{V64{16116372959101778759ull}, V64{17882202329957635143ull}}}}}},
                       V2{V4{V8{V16{V32{V64{6336991673848916940ull},  V64{13871375188855667794ull}},
                                    V32{V64{5511105481647804902ull},  V64{1480918079161433586ull}}},
                                V16{V32{V64{1022397212852227816ull},  V64{1319153298133843849ull}},
                                    V32{V64{10368706888717500276ull}, V64{310452456862315863ull}}}},
                             V8{V16{V32{V64{11519016341704165564ull}, V64{8158136662035505488ull}},
                                    V32{V64{10930546620055054253ull}, V64{2717032290008582875ull}}},
                                V16{V32{V64{9060964326400224025ull},  V64{15630566152903833874ull}},
                                    V32{V64{16257832541839974784ull}, V64{10609125650681273307ull}}}}},
                          V4{V8{V16{V32{V64{13826919200680275467ull}, V64{3341907900848901575ull}},
                                    V32{V64{8010874585836847148ull},  V64{1552676406494117513ull}}},
                                V16{V32{V64{7995786353529625892ull},  V64{10324497530860632201ull}},
                                    V32{V64{10774070879548228029ull}, V64{14397183669823220516ull}}}},
                             V8{V16{V32{V64{11680993877953024301ull}, V64{2515177814291670603ull}},
                                    V32{V64{13322855601435777763ull}, V64{13215152276666777352ull}}},
                                V16{V32{V64{3627633105478435593ull},  V64{7340956893285287069ull}},
                                    V32{V64{5858465404993020341ull},  V64{3219371878783330391ull}}}}}}},
                     V{V2{V4{V8{V16{V32{V64{4908180953381132453ull},  V64{7772599328345431970ull}},
                                    V32{V64{1156445212904289307ull},  V64{15790133796118278895ull}}},
                                V16{V32{V64{14147867948090161031ull}, V64{17378386577906762694ull}},
                                    V32{V64{13085863695290686394ull}, V64{18207507208555021719ull}}}},
                             V8{V16{V32{V64{5448423870479034757ull},  V64{4872361322195334884ull}},
                                    V32{V64{12431331446707514279ull}, V64{9886345804096144506ull}}},
                                V16{V32{V64{12366781995529520139ull}, V64{2675623913277696443ull}},
                                    V32{V64{13544096164861484748ull}, V64{2310859120980559373ull}}}}},
                          V4{V8{V16{V32{V64{5827020735507000480ull},  V64{12821728357634014343ull}},
                                    V32{V64{18016396905930284089ull}, V64{3623922785970165995ull}}},
                                V16{V32{V64{4258327730327412542ull},  V64{12397628181986784662ull}},
                                    V32{V64{18255102616803694484ull}, V64{8060601391251461488ull}}}},
                             V8{V16{V32{V64{4961010461629855917ull},  V64{16551650809778921684ull}},
                                    V32{V64{5121936562809948277ull},  V64{17544393596851498491ull}}},
                                V16{V32{V64{2021641221025914281ull},  V64{8664951787990288487ull}},
                                    V32{V64{18249583354227941245ull}, V64{17305003694283797309ull}}}}}},
                       V2{V4{V8{V16{V32{V64{6972475936725210572ull},  V64{16887086445537873707ull}},
                                    V32{V64{7405028997103298401ull},  V64{10881822116504057546ull}}},
                                V16{V32{V64{15973852921963938866ull}, V64{11730454780140056181ull}},
                                    V32{V64{80572034905818491ull},    V64{14118689724783216089ull}}}},
                             V8{V16{V32{V64{1170759298488670659ull},  V64{1853193021177027742ull}},
                                    V32{V64{10096770816515090979ull}, V64{4482513941139470123ull}}},
                                V16{V32{V64{4265584502797198650ull},  V64{17647444635871665453ull}},
                                    V32{V64{3664590262599264015ull},  V64{6247917156771729378ull}}}}},
                          V4{V8{V16{V32{V64{12741644143661320252ull}, V64{387361783615428304ull}},
                                    V32{V64{4221209422803042293ull},  V64{12068856759815409528ull}}},
                                V16{V32{V64{9755208595958615377ull},  V64{16478013757238604928ull}},
                                    V32{V64{15197428134291962796ull}, V64{10512815089066580045ull}}}},
                             V8{V16{V32{V64{9669642887185789934ull},  V64{14343612504784947754ull}},
                                    V32{V64{18075118312737742718ull}, V64{5986990677816560094ull}}},
                                V16{V32{V64{16592718875581723370ull}, V64{5422294176083223422ull}},
                                    V32{V64{898289490895134617ull},   V64{15919507916464698233ull}}}}}}}};
        }
        else
        {
            //             ----
            // sqrt3 = [1; 1, 2]
        
            constexpr auto M = std::numeric_limits<V>::max();
        
            V p{1};
            V q{1};
        
            V pm2{1};
            V pm1 = p;
            V qm2{0};
            V qm1 = q;
        
            bool two = false;
        
            while (true)
            {
                if (two)
                {
                    if (pm1 > ((M - pm2) >> 1))
                        break;
                    p = (pm1 << 1) + pm2;
                    q = (qm1 << 1) + qm2;
                }
                else
                {
                    if (pm1 > M - pm2)
                        break;
                    p = pm1 + pm2;
                    q = qm1 + qm2;
                }
        
                pm2 = pm1;
                pm1 = p;
                qm2 = qm1;
                qm1 = q;
                two = !two;
            }
            return R{p, q};
        }
    }();

template <unsigned N>
constexpr auto inv_sqrt3 = []()
    {
        return reciprocal(sqrt3<N>);
    }();


template <unsigned N>
constexpr auto sqrt5 = []()
    {
        using R = rational<N>;
        using V = typename R::value_type;
        if constexpr (N == 8)
        {
            return R{V{123}, V{55}};
        }
        else if constexpr (N == 16)
        {
            return R{V{12238}, V{5473}};
        }
        else if constexpr (N == 32)
        {
            return R{V{1268860318}, V{567451585}};
        }
        else if constexpr (N == 64)
        {
            return R{V{7200167985927040300}, V{3220013013190122249}};
        }
        else if constexpr (N == 128)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            return R{V{V2{4761989654820968127ull}, V2{5533880681561198170ull}},
                     V{V2{2129626515266088772ull}, V2{5459386094357288209ull}}};
        }
        else if constexpr (N == 256)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            return R{V{V2{V4{7956183741807906207ull},  V4{15556398894636554617ull}},
                       V2{V4{1943139053002146124ull},  V4{2005607856383806221ull}}},
                     V{V2{V4{3558113537632222762ull},  V4{13546247192960911575ull}},
                       V2{V4{16387055819095874687ull}, V4{15586334566667950909ull}}}};
        }
        else if constexpr (N == 512)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            return R{V{V2{V4{V8{8983905324264992841ull},  V8{10393215880550563494ull}},
                          V4{V8{6969015587050676091ull},  V8{7601165548259105011ull}}},
                       V2{V4{V8{9500785951763287762ull},  V8{4826643810705350194ull}},
                          V4{V8{7145665604244793662ull},  V8{13491599050572189887ull}}}},
                     V{V2{V4{V8{4017724601695762973ull},  V8{17477743522418975050ull}},
                          V4{V8{6084106054633720492ull},  V8{1234526015038827834ull}}},
                       V2{V4{V8{2954677974016290953ull},  V8{11378543557876294251ull}},
                          V4{V8{12945902499222113505ull}, V8{7415982149307855099ull}}}}};
        }
        else if constexpr (N == 1024)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            return R{V{V2{V4{V8{V16{3539714665020813785ull},  V16{9148631063113017798ull}},
                             V8{V16{922160776615613514ull},   V16{9021577176230397029ull}}},
                          V4{V8{V16{3379735857235335715ull},  V16{11664865661701376257ull}},
                             V8{V16{8309967575017409894ull},  V16{5437984230728394132ull}}}},
                       V2{V4{V8{V16{1906890340742854009ull},  V16{223941414738300725ull}},
                             V8{V16{8083184973104811282ull},  V16{11368292181622756325ull}}},
                          V4{V8{V16{4096167425755669969ull},  V16{9595791580986453560ull}},
                             V8{V16{3131197702994094381ull},  V16{5711002658929116598ull}}}}},
                     V{V2{V4{V8{V16{1583008522387887332ull},  V16{7502251750816452065ull}},
                             V8{V16{5179862571115646575ull},  V16{6908022060444209596ull}}},
                          V4{V8{V16{17564884544919819205ull}, V16{11209291881313570508ull}},
                             V8{V16{1309397418633202456ull},  V16{17197625166698880914ull}}}},
                       V2{V4{V8{V16{18437916412587721724ull}, V16{17382822105687302798ull}},
                             V8{V16{14403777990782051380ull}, V16{15222056777737565143ull}}},
                          V4{V8{V16{18368948713229259829ull}, V16{10323458453326197084ull}},
                             V8{V16{13746790166569937715ull}, V16{479974919852575345ull}}}}}};
        }
        else if constexpr (N == 2048)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            using V32 = Z<Unsigned, V::size/32, Wrap>;
            return R{V{V2{V4{V8{V16{V32{5754527955198126701ull},  V32{8841890500118765796ull}},
                                V16{V32{17453746883522138383ull}, V32{14524714613596355700ull}}},
                             V8{V16{V32{10592692557638167462ull}, V32{4652145720403555869ull}},
                                V16{V32{5780581626150811096ull},  V32{8852547299958765397ull}}}},
                          V4{V8{V16{V32{17417348148621354692ull}, V32{8405347241749703896ull}},
                                V16{V32{15345673648750706920ull}, V32{13101919346723333258ull}}},
                             V8{V16{V32{18430802912758168278ull}, V32{8240747552972151179ull}},
                                V16{V32{1805917037170307602ull},  V32{1773062051885011685ull}}}}},
                       V2{V4{V8{V16{V32{17980974410769447679ull}, V32{6509769535138402458ull}},
                                V16{V32{5239776299224553332ull},  V32{13218386123142956164ull}}},
                             V8{V16{V32{18110402388695662314ull}, V32{9026035010443091176ull}},
                                V16{V32{6951987149834186299ull},  V32{329466149197274685ull}}}},
                          V4{V8{V16{V32{4432577244158083740ull},  V32{7925450990014581032ull}},
                                V16{V32{221647395510426936ull},   V32{8990079998507279335ull}}},
                             V8{V16{V32{17452633181925358378ull}, V32{562751377955051709ull}},
                                V16{V32{18443014312997614909ull}, V32{10660833713712775918ull}}}}}},
                     V{V2{V4{V8{V16{V32{2573503137249175117ull},  V32{8386333725427536228ull}},
                                V16{V32{14390552531892364043ull}, V32{10957120364637533660ull}}},
                             V8{V16{V32{5156034152594162004ull},  V32{12701614769945675959ull}},
                                V16{V32{17339685262424119827ull}, V32{9660448953666257435ull}}}},
                          V4{V8{V16{V32{17783416796641867232ull}, V32{18105503202986397863ull}},
                                V16{V32{9942300806466271795ull},  V32{18385939769795445877ull}}},
                             V8{V16{V32{3401966312819387964ull},  V32{17594177768100673644ull}},
                                V16{V32{14775301167259167538ull}, V32{2422556727805955796ull}}}}},
                       V2{V4{V8{V16{V32{15617034745343572015ull}, V32{18216768848992838746ull}},
                                V16{V32{14748296316935495498ull}, V32{13621486051678217685ull}}},
                             V8{V16{V32{383586788025503953ull},   V32{7208834468315106660ull}},
                                V16{V32{10559020016480555046ull}, V32{15859261015226705553ull}}}},
                          V4{V8{V16{V32{7940285548089410516ull},  V32{1408705977848014700ull}},
                                V16{V32{14501185302102904033ull}, V32{11836571978033134904ull}}},
                             V8{V16{V32{6846104390779254235ull},  V32{16601700797034842965ull}},
                                V16{V32{991610844636040018ull},   V32{3367644986801766433ull}}}}}}};
        }
        else if constexpr (N == 4096)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            using V32 = Z<Unsigned, V::size/32, Wrap>;
            using V64 = Z<Unsigned, V::size/64, Wrap>;
            return R{V{V2{V4{V8{V16{V32{V64{8028135310445807833ull},  V64{5045821707422427492ull}},
                                    V32{V64{2585352477351955776ull},  V64{13767844024675302447ull}}},
                                V16{V32{V64{14670782104499145638ull}, V64{12143332489265510671ull}},
                                    V32{V64{5814128717822432057ull},  V64{16399713191152138612ull}}}},
                             V8{V16{V32{V64{11671924666566248307ull}, V64{18403556333529921192ull}},
                                    V32{V64{14413417993704756089ull}, V64{6087641948921143436ull}}},
                                V16{V32{V64{9351138685693767399ull},  V64{2871949878266624361ull}},
                                    V32{V64{1790946837397944845ull},  V64{13368556310288647041ull}}}}},
                          V4{V8{V16{V32{V64{931556117267553611ull},   V64{15773845939440808109ull}},
                                    V32{V64{2743295501466414838ull},  V64{5231291846097313036ull}}},
                                V16{V32{V64{6420357352392489045ull},  V64{2058235378139620093ull}},
                                    V32{V64{5689725101565188313ull},  V64{624901021975962826ull}}}},
                             V8{V16{V32{V64{14958751226968145016ull}, V64{4690023334022006181ull}},
                                    V32{V64{9822596094812608598ull},  V64{12203059630925922997ull}}},
                                V16{V32{V64{13467697322331161549ull}, V64{18334104048449242806ull}},
                                    V32{V64{7593984374706082969ull},  V64{4083322266886596422ull}}}}}},
                       V2{V4{V8{V16{V32{V64{5595895428475790194ull},  V64{6113316066975750721ull}},
                                    V32{V64{7318572666745535901ull},  V64{11838612090574179023ull}}},
                                V16{V32{V64{641390912658405523ull},   V64{16881386714471732478ull}},
                                    V32{V64{10205995989771508464ull}, V64{12054242284640227522ull}}}},
                             V8{V16{V32{V64{6618957043279745812ull},  V64{8120654171638550108ull}},
                                    V32{V64{9095132346210290078ull},  V64{15486586096594322783ull}}},
                                V16{V32{V64{16599805258282660302ull}, V64{2989024804796678853ull}},
                                    V32{V64{14590265908464903872ull}, V64{4299636911862315605ull}}}}},
                          V4{V8{V16{V32{V64{5118015807518555940ull},  V64{5462281576219807502ull}},
                                    V32{V64{15544501658489419491ull}, V64{460545765841065836ull}}},
                                V16{V32{V64{13282708944186514148ull}, V64{5802947754746750940ull}},
                                    V32{V64{7840011845850205706ull},  V64{470301438323837415ull}}}},
                             V8{V16{V32{V64{17536053274133157385ull}, V64{781441077907471260ull}},
                                    V32{V64{6689462479357671816ull},  V64{311424199201211181ull}}},
                                V16{V32{V64{12380471635105697019ull}, V64{16908193358020396864ull}},
                                    V32{V64{7844220005580495806ull},  V64{13094862039427204812ull}}}}}}},
                     V{V2{V4{V8{V16{V32{V64{3590291257344640759ull},  V64{16300218850409303990ull}},
                                    V32{V64{8166473799592911504ull},  V64{10435960336346498565ull}}},
                                V16{V32{V64{12067078658693816276ull}, V64{12289566924368488728ull}},
                                    V32{V64{17935396907785982806ull}, V64{5408847031410412678ull}}}},
                             V8{V16{V32{V64{10869459062320689957ull}, V64{15255863029951880337ull}},
                                    V32{V64{12691120070854583756ull}, V64{3283154191005807766ull}}},
                                V16{V32{V64{2132087101200732262ull},  V64{8387961965674199186ull}},
                                    V32{V64{13100551493018222303ull}, V64{8663460737812623224ull}}}}},
                          V4{V8{V16{V32{V64{14061445876747311561ull}, V64{13322531013427301685ull}},
                                    V32{V64{15979445689728438506ull}, V64{10324636219540881373ull}}},
                                V16{V32{V64{12815045000163064518ull}, V64{7082878299128812250ull}},
                                    V32{V64{1375349134423733284ull},  V64{13813620664976667944ull}}}},
                             V8{V16{V32{V64{16746581219603101312ull}, V64{11732108425367235536ull}},
                                    V32{V64{6362014207534104795ull},  V64{2742769822801204325ull}}},
                                V16{V32{V64{17041452474180067081ull}, V64{11617121840293004725ull}},
                                    V32{V64{168501181525775872ull},   V64{3033902843301956993ull}}}}}},
                       V2{V4{V8{V16{V32{V64{3883613612575942303ull},  V64{7088891151672641058ull}},
                                    V32{V64{9322129949236296858ull},  V64{7088937321529051650ull}}},
                                V16{V32{V64{4447450178319737750ull},  V64{9818832808322529846ull}},
                                    V32{V64{13404323292025155021ull}, V64{14226230521045082676ull}}}},
                             V8{V16{V32{V64{17243415668314451023ull}, V64{12080571614374353736ull}},
                                    V32{V64{14892044997858734353ull}, V64{16881001924793867806ull}}},
                                V16{V32{V64{13810973157722424391ull}, V64{3403728774868396711ull}},
                                    V32{V64{15168252434194499688ull}, V64{1224632702408891959ull}}}}},
                          V4{V8{V16{V32{V64{12455320217524237210ull}, V64{7938227079109323432ull}},
                                    V32{V64{17890905587624664628ull}, V64{4872539952552899921ull}}},
                                V16{V32{V64{9171029849182113157ull},  V64{5584672528482510779ull}},
                                    V32{V64{2710647854680803902ull},  V64{3311466241646820758ull}}}},
                             V8{V16{V32{V64{18436030688738496167ull}, V64{1155617449223241349ull}},
                                    V32{V64{15652129436376418603ull}, V64{15197510699596519166ull}}},
                                V16{V32{V64{814512603846976283ull},   V64{9227468542235566206ull}},
                                    V32{V64{8728523284432715134ull},  V64{5520683153255246473ull}}}}}}}};
        }
        else
        {
            //             -
            // sqrt5 = [2; 4]
        
            constexpr auto M = std::numeric_limits<V>::max();
        
            V p{2};
            V q{1};
        
            V pm2{1};
            V pm1 = p;
            V qm2{0};
            V qm1 = q;
        
            while (true)
            {
                if (pm1 > ((M - pm2) >> 2))
                    break;
                p = (pm1 << 2) + pm2;
                q = (qm1 << 2) + qm2;
        
                pm2 = pm1;
                pm1 = p;
                qm2 = qm1;
                qm1 = q;
            }
            return R{p, q};
        }
    }();

template <unsigned N>
constexpr auto sqrt7 = []()
    {
        using R = rational<N>;
        using V = typename R::value_type;
        if constexpr (N == 8)
        {
            return R{V{127}, V{48}};
        }
        else if constexpr (N == 16)
        {
            return R{V{32257}, V{12192}};
        }
        else if constexpr (N == 32)
        {
            return R{V{2081028097}, V{786554688}};
        }
        else if constexpr (N == 64)
        {
            return R{V{8661355881006882817}, V{3273684811110137472}};
        }
        else if constexpr (N == 128)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            return R{V{V2{8133585569105425060ull}, V2{9869578448649814017ull}},
                     V{V2{3074206383302387506ull}, V2{10347970540588727552ull}}};
        }
        else if constexpr (N == 256)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            return R{V{V2{V4{7172562696768256891ull},  V4{14988052821537108205ull}},
                       V2{V4{16736440308473093889ull}, V4{4972915151955951617ull}}},
                     V{V2{V4{2710973879809655884ull},  V4{17634667796014735015ull}},
                       V2{V4{8599125545281866216ull},  V4{14785075012859525632ull}}}};
        }
        else if constexpr (N == 512)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            return R{V{V2{V4{V8{5577749160882249466ull},  V8{7227973441278725970ull}},
                          V4{V8{16636620904629744698ull}, V8{14108977120124148570ull}}},
                       V2{V4{V8{14422743528728884791ull}, V8{10692666096605049361ull}},
                          V4{V8{15118298700412312703ull}, V8{16522735024445325313ull}}}},
                     V{V2{V4{V8{2108191022170518793ull},  V8{8713474714192793947ull}},
                          V4{V8{2626478613768605643ull},  V8{13144513841688253111ull}}},
                       V2{V4{V8{7613481071167235897ull},  V8{10438287985046298007ull}},
                          V4{V8{13205568191871714784ull}, V8{16247661227734332416ull}}}}};
        }
        else if constexpr (N == 1024)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            return R{V{V2{V4{V8{V16{8924363519690228706ull}, V16{4245432490571274307ull}},
                             V8{V16{17423695197300841432ull}, V16{13870724356681209828ull}}},
                          V4{V8{V16{3145151657391871994ull}, V16{8526653365656356662ull}},
                             V8{V16{10367340938938648129ull}, V16{1140652483964190659ull}}}},
                       V2{V4{V8{V16{14539574575082830313ull}, V16{12401834301715647791ull}},
                             V8{V16{4663400156788167805ull}, V16{5924639904739434498ull}}},
                          V4{V8{V16{2839515379836940765ull}, V16{279509299228888116ull}},
                             V8{V16{5561243957685451055ull}, V16{17421878919889639424ull}}}}},
                     V{V2{V4{V8{V16{3373092354662489546ull}, V16{2598647562970610614ull}},
                             V8{V16{6958440210482091259ull}, V16{5172219092903574331ull}}},
                          V4{V8{V16{5961191705643053975ull}, V16{7918329616426035804ull}},
                             V8{V16{6753921611613224290ull}, V16{13627791151649971118ull}}}},
                       V2{V4{V8{V16{4019979484206749004ull}, V16{18288970712664968265ull}},
                             V8{V16{15137335270322301859ull}, V16{9299780018655328456ull}}},
                          V4{V8{V16{5317079151603128303ull}, V16{3877846671378234593ull}},
                             V8{V16{5945037693201038588ull}, V16{7504387151260286977ull}}}}}};
        }
        else if constexpr (N == 2048)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            using V32 = Z<Unsigned, V::size/32, Wrap>;
            return R{V{V2{V4{V8{V16{V32{6964476571157417078ull},  V32{2205414563427442423ull}},
                                V16{V32{9599308823215119573ull},  V32{18444962220260283025ull}}},
                             V8{V16{V32{932229625113216639ull},   V32{5810708396385707815ull}},
                                V16{V32{6788191668913203577ull},  V32{10892183208746993669ull}}}},
                          V4{V8{V16{V32{3054679722621998297ull},  V32{17201515711927231937ull}},
                                V16{V32{3469434474184949739ull},  V32{11835809328982071217ull}}},
                             V8{V16{V32{7618836099409375854ull},  V32{10743325614483627526ull}},
                                V16{V32{9968359569401500205ull},  V32{903016058348527911ull}}}}},
                       V2{V4{V8{V16{V32{895495996697100472ull},   V32{8744092388107316517ull}},
                                V16{V32{2461315339294665913ull},  V32{7056208095017653720ull}}},
                             V8{V16{V32{9076481019618840714ull},  V32{11266633246088827323ull}},
                                V16{V32{6280466880314951270ull},  V32{35110473518522783ull}}}},
                          V4{V8{V16{V32{5266019740165600074ull},  V32{266685255890078834ull}},
                                V16{V32{16044137939360237014ull}, V32{5794207458875350499ull}}},
                             V8{V16{V32{4671918921759019046ull},  V32{2392023644964194247ull}},
                                V16{V32{2287035624023022042ull},  V32{5844700412577755139ull}}}}}},
                     V{V2{V4{V8{V16{V32{2632324717002622953ull},  V32{13372123284974640907ull}},
                                V16{V32{17235798111463116593ull}, V32{6794877238777553088ull}}},
                             V8{V16{V32{1722019285580106841ull},  V32{9561469526988610995ull}},
                                V16{V32{8337904582696321549ull},  V32{14647307238357912644ull}}}},
                          V4{V8{V16{V32{13793749397109026830ull}, V32{10389505776706565667ull}},
                                V16{V32{16930488545532279505ull}, V32{2008209555795563132ull}}},
                             V8{V16{V32{13732154989069034617ull}, V32{11744367002219861553ull}},
                                V16{V32{13457320862699331679ull}, V32{10437604979493760897ull}}}}},
                       V2{V4{V8{V16{V32{6375124666596622422ull},  V32{9820158767605633357ull}},
                                V16{V32{3146633739697681955ull},  V32{9828699586043054883ull}}},
                             V8{V16{V32{6014717008645012707ull},  V32{18072839478408409244ull}},
                                V16{V32{3433466324602230259ull},  V32{6050826972279112180ull}}}},
                          V4{V8{V16{V32{9978807292503954506ull},  V32{14055879300091670551ull}},
                                V16{V32{7689431498251523124ull},  V32{11314204073714776928ull}}},
                             V8{V16{V32{18066524154413865376ull}, V32{10862045236158999966ull}},
                                V16{V32{14059215322348031348ull}, V32{15356875346354335745ull}}}}}}};
        }
        else if constexpr (N == 4096)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            using V32 = Z<Unsigned, V::size/32, Wrap>;
            using V64 = Z<Unsigned, V::size/64, Wrap>;
            return R{V{V2{V4{V8{V16{V32{V64{6956747826170551151ull},  V64{12618086294963744793ull}},
                                    V32{V64{10075655364577939922ull}, V64{17424812879958283557ull}}},
                                V16{V32{V64{2646547024188314931ull},  V64{9095829221180242824ull}},
                                    V32{V64{17774016596136738381ull}, V64{13757691160520256795ull}}}},
                             V8{V16{V32{V64{17672718807826875545ull}, V64{14890281726082275363ull}},
                                    V32{V64{3393279918629841456ull},  V64{8143317976758764483ull}}},
                                V16{V32{V64{10355808173681133929ull}, V64{2508109389867049876ull}},
                                    V32{V64{3932786530252709410ull},  V64{12811716278739714495ull}}}}},
                          V4{V8{V16{V32{V64{18282621317115462879ull}, V64{7623715370248949868ull}},
                                    V32{V64{512886129536358190ull},   V64{14259341420332660288ull}}},
                                V16{V32{V64{11393099555715747290ull}, V64{17464853183513475897ull}},
                                    V32{V64{5730180344790011377ull},  V64{451490118796488930ull}}}},
                             V8{V16{V32{V64{269647282243501353ull},   V64{7839753810923220393ull}},
                                    V32{V64{15654855822849369103ull}, V64{787626979373116379ull}}},
                                V16{V32{V64{9586714709900094017ull},  V64{16821303026109503444ull}},
                                    V32{V64{5370557892166402896ull},  V64{14093814137884650850ull}}}}}},
                       V2{V4{V8{V16{V32{V64{8897662683898358621ull},  V64{16784604104693663464ull}},
                                    V32{V64{14398327489851194931ull}, V64{5711939386329963410ull}}},
                                V16{V32{V64{17492871126783627124ull}, V64{15287628457041892581ull}},
                                    V32{V64{12123191457452157108ull}, V64{11878589685450228698ull}}}},
                             V8{V16{V32{V64{12970308392888292938ull}, V64{11002256739192863727ull}},
                                    V32{V64{16011064815215369440ull}, V64{9416732250677601100ull}}},
                                V16{V32{V64{7901362909767635535ull},  V64{9996848430284365134ull}},
                                    V32{V64{10768886957355163589ull}, V64{5335311512554176452ull}}}}},
                          V4{V8{V16{V32{V64{18435222529556411162ull}, V64{7499928754322215902ull}},
                                    V32{V64{6440097350390748281ull},  V64{10380170014284958019ull}}},
                                V16{V32{V64{2167976281297201769ull},  V64{18146427032796102028ull}},
                                    V32{V64{6295948429998880581ull},  V64{6872259146144751980ull}}}},
                             V8{V16{V32{V64{11713378761056913985ull}, V64{1799299429603108896ull}},
                                    V32{V64{5679010167439112700ull},  V64{11347735114423590501ull}}},
                                V16{V32{V64{3356154473228927317ull},  V64{12062615206610359302ull}},
                                    V32{V64{2604957310191088224ull},  V64{14870391665321508885ull}}}}}}},
                     V{V2{V4{V8{V16{V32{V64{2629403525976639467ull},  V64{589674230733775089ull}},
                                    V32{V64{9956115269469661414ull},  V64{7001236458142416331ull}}},
                                V16{V32{V64{13890967572537766204ull}, V64{14795734409267100033ull}},
                                    V32{V64{5419225313515586513ull},  V64{16039081783265882941ull}}}},
                             V8{V16{V32{V64{8685746892572546735ull},  V64{4196298984652933477ull}},
                                    V32{V64{8147475364551749779ull},  V64{4052510503883973449ull}}},
                                V16{V32{V64{4761899288282540955ull},  V64{12941671333402295618ull}},
                                    V32{V64{1224861576866656380ull},  V64{11211956826089327819ull}}}}},
                          V4{V8{V16{V32{V64{1016246434160788290ull},  V64{8624780163299749330ull}},
                                    V32{V64{4066268068967630932ull},  V64{17822472902872761744ull}}},
                                V16{V32{V64{8964558286363804444ull},  V64{11423183865011513182ull}},
                                    V32{V64{14507656208203409166ull}, V64{14218211519498031773ull}}}},
                             V8{V16{V32{V64{14739599026346814705ull}, V64{11059272318292481157ull}},
                                    V32{V64{5963106993169979186ull},  V64{2406412745696591529ull}}},
                                V16{V32{V64{7192744022628262616ull},  V64{10513008974166242220ull}},
                                    V32{V64{9448079271395411615ull},  V64{12623448440782391342ull}}}}}},
                       V2{V4{V8{V16{V32{V64{12386027691352511272ull}, V64{8634794167580799222ull}},
                                    V32{V64{17375477011180171972ull}, V64{10138670389036577570ull}}},
                                V16{V32{V64{4389761564786531692ull},  V64{6100575155108786068ull}},
                                    V32{V64{12273541386803838156ull}, V64{2849234289907267952ull}}}},
                             V8{V16{V32{V64{3480965924015978443ull},  V64{6306804146616975430ull}},
                                    V32{V64{14529950952869850151ull}, V64{1488099858458618612ull}}},
                                V16{V32{V64{9366435557052827671ull},  V64{14740473969786652519ull}},
                                    V32{V64{6068101072624621421ull},  V64{11211743222007860297ull}}}}},
                          V4{V8{V16{V32{V64{7415511059289506676ull},  V64{12560028286222019065ull}},
                                    V32{V64{2920028065846940131ull},  V64{8004318050804072134ull}}},
                                V16{V32{V64{6011277393276588183ull},  V64{6712496145885845005ull}},
                                    V32{V64{7981152670357672424ull},  V64{13898074915650826476ull}}}},
                             V8{V16{V32{V64{371009144482077060ull},   V64{16133897582212808531ull}},
                                    V32{V64{287286259013020203ull},   V64{5434776867360247391ull}}},
                                V16{V32{V64{9889835652412475455ull},  V64{473343587155095873ull}},
                                    V32{V64{5008410850210112947ull},  V64{8330335631220154376ull}}}}}}}};
        }
        else
        {
            //             ----------
            // sqrt7 = [2; 1, 1, 1, 4]
        
            constexpr auto M = std::numeric_limits<V>::max();
        
            V p{2};
            V q{1};
        
            V pm2{1};
            V pm1 = p;
            V qm2{0};
            V qm1 = q;
    
            unsigned four = 1;
        
            while (true)
            {
                if (four == 4)
                {
                    if (pm1 > ((M - pm2) >> 2))
                        break;
                    p = (pm1 << 2) + pm2;
                    q = (qm1 << 2) + qm2;
                    four = 0;
                }
                else
                {
                    if (pm1 > M - pm2)
                        break;
                    p = pm1 + pm2;
                    q = qm1 + qm2;
                }
        
                pm2 = pm1;
                pm1 = p;
                qm2 = qm1;
                qm1 = q;
                ++four;
            }
            return R{p, q};
        }
    }();

template <unsigned N>
constexpr
auto e = []()
    {
        using R = rational<N>;
        using V = typename R::value_type;
        if constexpr (N == 8)
        {
            return R{V{106}, V{39}};
        }
        else if constexpr (N == 16)
        {
            return R{V{25946}, V{9545}};
        }
        else if constexpr (N == 32)
        {
            return R{V{848456353}, V{312129649}};
        }
        else if constexpr (N == 64)
        {
            return R{V{5739439214861417731}, V{2111421691000680031}};
        }
        else if constexpr (N == 128)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            return R{V{V2{1783510482136599286ull}, V2{9709680529934793825ull}},
                     V{V2{656116839491821808ull},  V2{15048257969463281873ull}}};
        }
        else if constexpr (N == 256)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            return R{V{V2{V4{618642546146714785ull},  V4{17194564995802566290ull}},
                       V2{V4{2515889525427998378ull}, V4{16556728867995018131ull}}},
                     V{V2{V4{227585874161331653ull},  V4{14467807560987148449ull}},
                       V2{V4{2103281096109403833ull}, V4{5927804381294590967ull}}}};
        }
        else if constexpr (N == 512)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            return R{V{V2{V4{V8{5219471510476296288ull},  V8{9185705736710681275ull}},
                          V4{V8{9208363266065757527ull},  V8{5549960992911410481ull}}},
                       V2{V4{V8{5043448612675285339ull},  V8{673923323650211471ull}},
                          V4{V8{13206202650912759265ull}, V8{17371854178689506145ull}}}},
                     V{V2{V4{V8{1920136262484283835ull},  V8{11450907897091086058ull}},
                          V4{V8{9774144568850473326ull},  V8{9668119195452710050ull}}},
                       V2{V4{V8{9532986370701619886ull},  V8{17236408620050569170ull}},
                          V4{V8{8907056867669214727ull},  V8{6116007561168535927ull}}}}};
        }
        else if constexpr (N == 1024)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            return R{V{V2{V4{V8{V16{7783040363672404153ull},  V16{444392766209917988ull}},
                             V8{V16{6534448625013903995ull},  V16{11025932604040449900ull}}},
                          V4{V8{V16{3411233986997772974ull},  V16{2942052291666041167ull}},
                             V8{V16{5122098988120209537ull},  V16{10912705348428684915ull}}}},
                       V2{V4{V8{V16{8616574270762926485ull},  V16{4169659931912078001ull}},
                             V8{V16{11294369654490640546ull}, V16{7791312939491477718ull}}},
                          V4{V8{V16{14424003172137905811ull}, V16{2346873585476167797ull}},
                             V8{V16{15017510680312320425ull}, V16{17746475816763733059ull}}}}},
                     V{V2{V4{V8{V16{2863220539602583255ull},  V16{17713439778248177776ull}},
                             V8{V16{227307968909418812ull},   V16{12845857660471833590ull}}},
                          V4{V8{V16{8115000855089885286ull},  V16{16472900825072111594ull}},
                             V8{V16{2677857965348844581ull},  V16{13518669953294011014ull}}}},
                       V2{V4{V8{V16{12881679252789542928ull}, V16{16037093551584084417ull}},
                             V8{V16{13405510929792937051ull}, V16{15013740331512756014ull}}},
                          V4{V8{V16{3973536085161909701ull},  V16{5303393159337028307ull}},
                             V8{V16{12134550286602516527ull}, V16{2330538585606588783ull}}}}}};
        }
        else if constexpr (N == 2048)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            using V32 = Z<Unsigned, V::size/32, Wrap>;
            return R{V{V2{V4{V8{V16{V32{418367486207060805ull},   V32{11807999230847379936ull}},
                                V16{V32{13307334980210012824ull}, V32{10326587019307589195ull}}},
                             V8{V16{V32{18411699691882693817ull}, V32{8763794148212008862ull}},
                                V16{V32{15377552989125976956ull}, V32{9129739342527483217ull}}}},
                          V4{V8{V16{V32{13959966468897492582ull}, V32{8919098157074315236ull}},
                                V16{V32{15412764041102886792ull}, V32{3119862607313696619ull}}},
                             V8{V16{V32{16285501564369115305ull}, V32{6655975310947828924ull}},
                                V16{V32{6488546326937259584ull},  V32{4818953733334108704ull}}}}},
                       V2{V4{V8{V16{V32{3377405164348584545ull},  V32{543930649911817462ull}},
                                V16{V32{15484905618732113795ull}, V32{1382424029594938117ull}}},
                             V8{V16{V32{14529359133891305743ull}, V32{9752189607654893397ull}},
                                V16{V32{17574940358115332810ull}, V32{12837010360103825599ull}}}},
                          V4{V8{V16{V32{18255249972473211770ull}, V32{1871596794860305221ull}},
                                V16{V32{16239279619245649877ull}, V32{5081215167168848839ull}}},
                             V8{V16{V32{4554381441603066542ull},  V32{11871846429790967558ull}},
                                V16{V32{10105021393353612385ull}, V32{6198606069059788035ull}}}}}},
                     V{V2{V4{V8{V16{V32{153908797030154632ull},   V32{10096201174057793032ull}},
                                V16{V32{2816585904681208054ull},  V32{7814611606839323588ull}}},
                             V8{V16{V32{14594385262378732007ull}, V32{8598285109867210437ull}},
                                V16{V32{1231765096663811920ull},  V32{5480196425889173289ull}}}},
                          V4{V8{V16{V32{4843125004512168568ull},  V32{4290975638796429277ull}},
                                V16{V32{14404081308516766822ull}, V32{84625480193540001ull}}},
                             V8{V16{V32{3407575705604408200ull},  V32{4608673555252562180ull}},
                                V16{V32{4696149158150716764ull},  V32{11255439513149812483ull}}}}},
                       V2{V4{V8{V16{V32{14730714709415986052ull}, V32{1027160748387573708ull}},
                                V16{V32{15880569093878195941ull}, V32{3093188447687257650ull}}},
                             V8{V16{V32{10331699396877472152ull}, V32{15157252920952943025ull}},
                                V16{V32{212424097072505766ull},   V32{14098747786146499427ull}}}},
                          V4{V8{V16{V32{14078001000311426654ull}, V32{17130947149423923705ull}},
                                V16{V32{4038133071171882744ull},  V32{13752287223692337953ull}}},
                             V8{V16{V32{10471426105613840453ull}, V32{9454332960007822908ull}},
                                V16{V32{1569139269411201545ull},  V32{10063843297252911009ull}}}}}}};
        }
        else if constexpr (N == 4096)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            using V32 = Z<Unsigned, V::size/32, Wrap>;
            using V64 = Z<Unsigned, V::size/64, Wrap>;
            return R{V{V2{V4{V8{V16{V32{V64{115231149490994854ull},   V64{7374927680697746549ull}},
                                    V32{V64{10931258101771938172ull}, V64{14818136491337857469ull}}},
                                V16{V32{V64{9434230682586691610ull},  V64{15653763079472325504ull}},
                                    V32{V64{16899596836113049910ull}, V64{3532956223518421502ull}}}},
                             V8{V16{V32{V64{13173518022827128902ull}, V64{13211022382681870867ull}},
                                    V32{V64{18006427924238925052ull}, V64{9358454489815288012ull}}},
                                V16{V32{V64{6531286708308273331ull},  V64{7262091667525268583ull}},
                                    V32{V64{17889594104444295623ull}, V64{6930063119501985982ull}}}}},
                          V4{V8{V16{V32{V64{13112981673639541413ull}, V64{12476446948274136058ull}},
                                    V32{V64{7349973696372272523ull},  V64{16748259414898510042ull}}},
                                V16{V32{V64{14783263367803337576ull}, V64{14739074286503869310ull}},
                                    V32{V64{7640693665130623264ull},  V64{2557034815577956038ull}}}},
                             V8{V16{V32{V64{7718582521228283313ull},  V64{10594283088245116840ull}},
                                    V32{V64{1426528189010506818ull},  V64{14879357974590443560ull}}},
                                V16{V32{V64{15767918691080485986ull}, V64{6081181309124615877ull}},
                                    V32{V64{5397933758780882928ull},  V64{6389385364631757685ull}}}}}},
                       V2{V4{V8{V16{V32{V64{8740307819729674562ull},  V64{14223285115389430116ull}},
                                    V32{V64{14769938228474627672ull}, V64{4945307405656029507ull}}},
                                V16{V32{V64{15785559638081548296ull}, V64{9585586572183208606ull}},
                                    V32{V64{8006470147485465405ull},  V64{10030587339055495950ull}}}},
                             V8{V16{V32{V64{11883415356336901410ull}, V64{16361600483917073730ull}},
                                    V32{V64{18005893322932945064ull}, V64{4777206260160992618ull}}},
                                V16{V32{V64{8585559780521849188ull},  V64{11538058278593067656ull}},
                                    V32{V64{15979609664639776283ull}, V64{10392718639633599530ull}}}}},
                          V4{V8{V16{V32{V64{652679213296486125ull},   V64{9311414055314324307ull}},
                                    V32{V64{7326128144485721754ull},  V64{12187337028973936296ull}}},
                                V16{V32{V64{3765421246983734548ull},  V64{18260736984157726487ull}},
                                    V32{V64{3835005010894647069ull},  V64{10471489185932227047ull}}}},
                             V8{V16{V32{V64{16441109982460526674ull}, V64{15567743832581092916ull}},
                                    V32{V64{6211422792789369509ull},  V64{5786251155986703135ull}}},
                                V16{V32{V64{5907158014671157491ull},  V64{6605934617795265343ull}},
                                    V32{V64{13337723655674348340ull}, V64{11595215708127243331ull}}}}}}},
                     V{V2{V4{V8{V16{V32{V64{42391170880290117ull},    V64{6628472645880495465ull}},
                                    V32{V64{5405098604294068533ull},  V64{3363561760417012868ull}}},
                                V16{V32{V64{4476822835132107371ull},  V64{7354416828654114263ull}},
                                    V32{V64{9969948823927300018ull},  V64{10315933144170075993ull}}}},
                             V8{V16{V32{V64{15470166538284407518ull}, V64{2971974245610882742ull}},
                                    V32{V64{8967343104057657021ull},  V64{4889371831477423826ull}}},
                                V16{V32{V64{17507618493015345667ull}, V64{13279593227211559492ull}},
                                    V32{V64{3223722436722647736ull},  V64{4134125017042534459ull}}}}},
                          V4{V8{V16{V32{V64{11683007225603074386ull}, V64{17085254616433643558ull}},
                                    V32{V64{18152135779528552157ull}, V64{5538957647579574853ull}}},
                                V16{V32{V64{8982487753274766989ull},  V64{12468859818716605635ull}},
                                    V32{V64{2011626061587371207ull},  V64{15548396287795959443ull}}}},
                             V8{V16{V32{V64{5681076636603129396ull},  V64{10006980381019901587ull}},
                                    V32{V64{12496039227202599317ull}, V64{3868019261536110538ull}}},
                                V16{V32{V64{17941927255476737924ull}, V64{5956467067171018528ull}},
                                    V32{V64{16130150392671887355ull}, V64{5385628216382030933ull}}}}}},
                       V2{V4{V8{V16{V32{V64{18310768215614928390ull}, V64{15444845734770928270ull}},
                                    V32{V64{11613451102127526940ull}, V64{9222483571207308611ull}}},
                                V16{V32{V64{10595607020031942955ull}, V64{8837683506271889033ull}},
                                    V32{V64{6520874952020242242ull},  V64{3970026519865549861ull}}}},
                             V8{V16{V32{V64{9955010807734870630ull},  V64{13256113542652733620ull}},
                                    V32{V64{11598027137557398123ull}, V64{2461554656394459317ull}}},
                                V16{V32{V64{12804237382502513578ull}, V64{104388663252174200ull}},
                                    V32{V64{7089098454714119282ull},  V64{7799787574551645308ull}}}}},
                          V4{V8{V16{V32{V64{12872798258280559807ull}, V64{12988508836010709285ull}},
                                    V32{V64{14851125270642850068ull}, V64{4882529154014208397ull}}},
                                V16{V32{V64{2629175073554849612ull},  V64{2880501592363128394ull}},
                                    V32{V64{9353820778337134962ull},  V64{17406970179331916545ull}}}},
                             V8{V16{V32{V64{14678406930802852203ull}, V64{7655217933817782089ull}},
                                    V32{V64{4022898973912141480ull},  V64{8343864135151175269ull}}},
                                V16{V32{V64{12480847402248929933ull}, V64{418029125503489135ull}},
                                    V32{V64{8256201238128747208ull},  V64{8793703618445258127ull}}}}}}}};
        }
        else
        {
            // e = [2; 1, 2, 1, 1, 4, 1, 1, 6, 1, 1, 8, ...]
    
            constexpr auto M = std::numeric_limits<V>::max();
        
            V p{2};
            V q{1};
        
            V pm2{1};
            V pm1 = p;
            V qm2{0};
            V qm1 = q;
    
            // a = 1
            p = pm1 + pm2;
            q = qm1 + qm2;
    
            pm2 = pm1;
            pm1 = p;
            qm2 = qm1;
            qm1 = q;
    
            // a = 2
            p = (pm1 << 1) + pm2;
            q = (qm1 << 1) + qm2;
    
            pm2 = pm1;
            pm1 = p;
            qm2 = qm1;
            qm1 = q;
    
            unsigned count = 0;
            V a{4};
        
            while (true)
            {
                if (count == 2)
                {
                    if (pm1 > (M-pm2)/a)
                        break;
                    p = a*pm1 + pm2;
                    q = a*qm1 + qm2;
                    count = 0;
                    a += V{2};
                }
                else
                {
                    if (pm1 > M-pm2)
                        break;
                    p = pm1 + pm2;
                    q = qm1 + qm2;
                    ++count;
                }
        
                pm2 = pm1;
                pm1 = p;
                qm2 = qm1;
                qm1 = q;
            }
            return R{p, q};
        }
    }();

template <unsigned N>
constexpr
auto ln2 = []()
    {
        using R = rational<N>;
        using V = typename R::value_type;
        if constexpr (N == 8)
        {
            return R{V{61}, V{88}};
        }
        else if constexpr (N == 16)
        {
            return R{V{18419}, V{26573}};
        }
        else if constexpr (N == 32)
        {
            return R{V{497083768}, V{717140287}};
        }
        else if constexpr (N == 64)
        {
            return R{V{3052446177238342414}, V{4403748962482230453}};
        }
        else if constexpr (N == 128)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            return R{V{V2{1434805776301138759ull}, V2{15072019713063515590ull}},
                     V{V2{2069987178108492266ull}, V2{17343258525626097375ull}}};
        }
        else if constexpr (N == 256)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            return R{V{V2{V4{1776191563937476047ull},  V4{2716847559171538997ull}},
                       V2{V4{12484945611022513917ull}, V4{9753110229659938467ull}}},
                     V{V2{V4{2562502760961408868ull},  V4{2582545957488641707ull}},
                       V2{V4{3931634176858876481ull},  V4{5024322176126295861ull}}}};
        }
        else if constexpr (N == 512)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            return R{V{V2{V4{V8{2890840377239063909ull},  V8{8069514814284785101ull}},
                          V4{V8{11652970798183150726ull}, V8{10186983083190483896ull}}},
                       V2{V4{V8{13969318954871737797ull}, V8{657324633906110816ull}},
                          V4{V8{11189127685039934287ull}, V8{2198361643721751046ull}}}},
                     V{V2{V4{V8{4170601076244377708ull},  V8{5026840430719750765ull}},
                          V4{V8{1567796535246107313ull},  V8{10808350510035777804ull}}},
                       V2{V4{V8{17819452158034757507ull}, V8{457866146954901784ull}},
                          V4{V8{2428617887242675804ull},  V8{11719489620462952903ull}}}}};
        }
        else if constexpr (N == 1024)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            return R{V{V2{V4{V8{V16{1493448549027502662ull},  V16{3491622666038676471ull}},
                             V8{V16{130166719989862647ull},   V16{1114576138130498997ull}}},
                          V4{V8{V16{6020424496484789189ull},  V16{8960962024269721447ull}},
                             V8{V16{15520625803097491716ull}, V16{12004873512818699255ull}}}},
                       V2{V4{V8{V16{3684020177987736657ull},  V16{1133476257180731622ull}},
                             V8{V16{8465735560196664015ull},  V16{9593215002094179746ull}}},
                          V4{V8{V16{13557138527092722291ull}, V16{8016123507938055073ull}},
                             V8{V16{5140175970733515053ull},  V16{9782421843581814087ull}}}}},
                     V{V2{V4{V8{V16{2154590815504796025ull},  V16{3458688656529251339ull}},
                             V8{V16{9138723379741963848ull},  V16{6581628768331158228ull}}},
                          V4{V8{V16{12941974940835301297ull}, V16{11301115780402957073ull}},
                             V8{V16{3286109892479835125ull},  V16{4382742994720878709ull}}}},
                       V2{V4{V8{V16{7816204304171733556ull},  V16{13358683913007042196ull}},
                             V8{V16{6519362967576827929ull},  V16{12671556877075072014ull}}},
                          V4{V8{V16{14517513601591035831ull}, V16{10179564598400216270ull}},
                             V8{V16{4805444952259969318ull},  V16{10010001075363572798ull}}}}}};
        }
        else if constexpr (N == 2048)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            using V32 = Z<Unsigned, V::size/32, Wrap>;
            return R{V{V2{V4{V8{V16{V32{5489643730853285532ull},  V32{14760271476066898101ull}},
                                V16{V32{17517269637684989251ull}, V32{15812510487162820378ull}}},
                             V8{V16{V32{9505317532231754605ull},  V32{4538319118689245003ull}},
                                V16{V32{8808961788524985058ull},  V32{18170198775213073460ull}}}},
                          V4{V8{V16{V32{1040990994603731534ull},  V32{4626030636606712015ull}},
                                V16{V32{10323978572917333495ull}, V32{2645034369690957539ull}}},
                             V8{V16{V32{3195354580419474980ull},  V32{9849221905652477882ull}},
                                V16{V32{16816255417446333548ull}, V32{5492972147790766745ull}}}}},
                       V2{V4{V8{V16{V32{17407120928475283859ull}, V32{3904199563024632990ull}},
                                V16{V32{9071550890067393125ull},  V32{2547274683213223385ull}}},
                             V8{V16{V32{10515814198159896840ull}, V32{18101782172952396878ull}},
                                V16{V32{15705369195294629121ull}, V32{15199889185460109198ull}}}},
                          V4{V8{V16{V32{10369240630506824592ull}, V32{15617764921496234989ull}},
                                V16{V32{2718680097236666683ull},  V32{15842366268515529956ull}}},
                             V8{V16{V32{9977064869194240789ull},  V32{9684242069584846890ull}},
                                V16{V32{12753871806943700842ull}, V32{12229164914559169264ull}}}}}},
                     V{V2{V4{V8{V16{V32{7919881786749222402ull},  V32{847226675229502697ull}},
                                V16{V32{729376583299591163ull},   V32{2065498977877897995ull}}},
                             V8{V16{V32{17302791544097608954ull}, V32{9046074211393567505ull}},
                                V16{V32{2279659930285899338ull},  V32{10574737019542979579ull}}}},
                          V4{V8{V16{V32{3178094916210918840ull},  V32{4285825685035123015ull}},
                                V16{V32{17116989194811097800ull}, V32{6404476869096490105ull}}},
                             V8{V16{V32{18304021403138630441ull}, V32{12584874305365433111ull}},
                                V16{V32{15869712919206348437ull}, V32{16322198267270049346ull}}}}},
                       V2{V4{V8{V16{V32{5873597239952565844ull},  V32{13776311709277297297ull}},
                                V16{V32{1181031416737745731ull},  V32{17804584782996046395ull}}},
                             V8{V16{V32{14340274397314404093ull}, V32{5199816342355598352ull}},
                                V16{V32{179891196247321292ull},   V32{9164541281869145451ull}}}},
                          V4{V8{V16{V32{16152031266798664316ull}, V32{8619215789353770836ull}},
                                V16{V32{2083327584928241090ull},  V32{5551458630230393790ull}}},
                             V8{V16{V32{10188041624425884108ull}, V32{14213012733836415357ull}},
                                V16{V32{3253264941491739518ull},  V32{16772393664590331329ull}}}}}}};
        }
        else if constexpr (N == 4096)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            using V32 = Z<Unsigned, V::size/32, Wrap>;
            using V64 = Z<Unsigned, V::size/64, Wrap>;
            return R{V{V2{V4{V8{V16{V32{V64{5371301582078395640ull},  V64{11088372526568116941ull}},
                                    V32{V64{142767084232325419ull},   V64{17593731301213204608ull}}},
                                V16{V32{V64{12811849583554563482ull}, V64{10776562278520476226ull}},
                                    V32{V64{15812967593568609297ull}, V64{2303085959977438812ull}}}},
                             V8{V16{V32{V64{8523833613466392216ull},  V64{9444649689784711195ull}},
                                    V32{V64{6580687423460787253ull},  V64{820997159328538034ull}}},
                                V16{V32{V64{2985217224599238178ull},  V64{8291203691659055983ull}},
                                    V32{V64{2282694280483111798ull},  V64{13972439389637610572ull}}}}},
                          V4{V8{V16{V32{V64{15296626052667303382ull}, V64{12757288367741156553ull}},
                                    V32{V64{1951214312864194497ull},  V64{16158496668655607850ull}}},
                                V16{V32{V64{4374540988516583561ull},  V64{517955478916464117ull}},
                                    V32{V64{9515567575970695452ull},  V64{14301189988051552013ull}}}},
                             V8{V16{V32{V64{5249166488981012093ull},  V64{11233907090486758556ull}},
                                    V32{V64{14714981463329326057ull}, V64{15109509365114725443ull}}},
                                V16{V32{V64{6686511600108057506ull},  V64{7100710623892925367ull}},
                                    V32{V64{13271885611368567862ull}, V64{10495372062933343177ull}}}}}},
                       V2{V4{V8{V16{V32{V64{11036686492986653321ull}, V64{13247808214443918509ull}},
                                    V32{V64{11711808968304699610ull}, V64{3036258761205166020ull}}},
                                V16{V32{V64{7693279229182008637ull},  V64{7285162266592673612ull}},
                                    V32{V64{3311989683672852703ull},  V64{1607689423724986459ull}}}},
                             V8{V16{V32{V64{12402394785421075871ull}, V64{2027583882056392501ull}},
                                    V32{V64{500422032511399304ull},   V64{18230135699030205995ull}}},
                                V16{V32{V64{2777053083503497258ull},  V64{17740263295303815615ull}},
                                    V32{V64{11992782480402960761ull}, V64{6553475247179124876ull}}}}},
                          V4{V8{V16{V32{V64{11557986467066104397ull}, V64{6033250252450647079ull}},
                                    V32{V64{285526857900928921ull},   V64{8276655760175843657ull}}},
                                V16{V32{V64{432470866946227976ull},   V64{1784052266100682722ull}},
                                    V32{V64{5814296840045806073ull},  V64{14821654445510312795ull}}}},
                             V8{V16{V32{V64{1021123181970037866ull},  V64{1227300991249299530ull}},
                                    V32{V64{10131069037524809435ull}, V64{1864794452425626101ull}}},
                                V16{V32{V64{323343467621948338ull},   V64{13286290631856644509ull}},
                                    V32{V64{15928198939674079198ull}, V64{6760497378729279391ull}}}}}}},
                     V{V2{V4{V8{V16{V32{V64{7749150155583544838ull},  V64{3965995333182502880ull}},
                                    V32{V64{15295243078438936195ull}, V64{14341904579062601381ull}}},
                                V16{V32{V64{4849691109892995460ull},  V64{13357075789140542550ull}},
                                    V32{V64{18401496683831094762ull}, V64{13383882078435533291ull}}}},
                             V8{V16{V32{V64{10796914102334442152ull}, V64{18115801429471016643ull}},
                                    V32{V64{11389296220515520805ull}, V64{2458954366160274210ull}}},
                                V16{V32{V64{16591851708075827485ull}, V64{9180089213758829759ull}},
                                    V32{V64{793392023298293501ull},   V64{6798714427009134683ull}}}}},
                          V4{V8{V16{V32{V64{4284231694614068112ull},  V64{377509085870149965ull}},
                                    V32{V64{11447055125238420066ull}, V64{14319157916726201460ull}}},
                                V16{V32{V64{978397470541183193ull},   V64{9093099452810316668ull}},
                                    V32{V64{15664984099019635156ull}, V64{11173699876880738437ull}}}},
                             V8{V16{V32{V64{2776376411927858230ull},  V64{7706701157721336486ull}},
                                    V32{V64{3527034420938410054ull},  V64{9655161374811188533ull}}},
                                V16{V32{V64{14480955298497019911ull}, V64{3718087053594138444ull}},
                                    V32{V64{483729748384248071ull},   V64{9228103441422802011ull}}}}}},
                       V2{V4{V8{V16{V32{V64{8659058165621428694ull},  V64{11429986859559634483ull}},
                                    V32{V64{5754816080819620687ull},  V64{87763104964382068ull}}},
                                V16{V32{V64{3863879612040416924ull},  V64{8146296640224059829ull}},
                                    V32{V64{6562144823370280335ull},  V64{16118045945343411413ull}}}},
                             V8{V16{V32{V64{12218598540463942872ull}, V64{16683605097329968700ull}},
                                    V32{V64{5115098927967333454ull},  V64{10736146378460856619ull}}},
                                V16{V32{V64{11605142313098915320ull}, V64{335354328652485789ull}},
                                    V32{V64{16341112151589297382ull}, V64{5145857780659627826ull}}}}},
                          V4{V8{V16{V32{V64{3397051978082842564ull},  V64{7998867494212249214ull}},
                                    V32{V64{8810353992058279399ull},  V64{15055463434030662397ull}}},
                                V16{V32{V64{4296125300864565801ull},  V64{2445646682636618530ull}},
                                    V32{V64{2728874575272849130ull},  V64{11398098134391734978ull}}}},
                             V8{V16{V32{V64{1800591998177654853ull},  V64{11067579464106807134ull}},
                                    V32{V64{5937453711059491598ull},  V64{13475127903853482624ull}}},
                                V16{V32{V64{14649838990708322566ull}, V64{4062394524908742266ull}},
                                    V32{V64{11581795598710283475ull}, V64{15950772615626055705ull}}}}}}}};
        }
        else
        {
            throw "higher bit widths for ln2 not supported";
        }
    }();

template <unsigned N>
constexpr
auto log2e = []()
    {
        return reciprocal(ln2<N>);
    }();

template <unsigned N>
constexpr
auto ln10 = []()
    {
        using R = rational<N>;
        using V = typename R::value_type;
        if constexpr (N == 8)
        {
            return R{V{99}, V{43}};
        }
        else if constexpr (N == 16)
        {
            return R{V{28681}, V{12456}};
        }
        else if constexpr (N == 32)
        {
            return R{V{1784326399}, V{774923109}};
        }
        else if constexpr (N == 64)
        {
            return R{V{7013186985772460987}, V{3045788408476679291}};
        }
        else if constexpr (N == 128)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            return R{V{V2{6175330123558849229ull}, V2{12741890572455377546ull}},
                     V{V2{2681911796592534519ull}, V2{11425760884915152683ull}}};
        }
        else if constexpr (N == 256)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            return R{V{V2{V4{7320174150770922604ull},  V4{10410556298131332934ull}},
                       V2{V4{17323156878480923665ull}, V4{6739782791170057588ull}}},
                     V{V2{V4{3179111240250634262ull},  V4{15658190445999275572ull}},
                       V2{V4{2974025255160973565ull},  V4{15234924257444655775ull}}}};
        }
        else if constexpr (N == 512)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            return R{V{V2{V4{V8{8806348186319931740ull},  V8{3352630087122743071ull}},
                          V4{V8{9280774886404884477ull},  V8{8004251653077722113ull}}},
                       V2{V4{V8{3311019382314524951ull},  V8{695085946041561023ull}},
                          V4{V8{17634079144659202409ull}, V8{6750116541529892065ull}}}},
                     V{V2{V4{V8{3824548423037456149ull},  V8{5226721520048730646ull}},
                          V4{V8{17303762135570445425ull}, V8{2090505267175546094ull}}},
                       V2{V4{V8{1346539146508546229ull},  V8{1479312842791225289ull}},
                          V4{V8{12334254244896512173ull}, V8{14727553634456805436ull}}}}};
        }
        else if constexpr (N == 1024)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            return R{V{V2{V4{V8{V16{6282773398830997309ull},  V16{689281304193548863ull}},
                             V8{V16{8654664520306853531ull},  V16{10127668320815195036ull}}},
                          V4{V8{V16{1190370810628878348ull},  V16{10324545240690320891ull}},
                             V8{V16{660983125698209726ull},   V16{2524691094056195578ull}}}},
                       V2{V4{V8{V16{7755387286464400063ull},  V16{16123096925281047532ull}},
                             V8{V16{8011899825777587538ull},  V16{11876578353333000032ull}}},
                          V4{V8{V16{7235552351061530379ull},  V16{3444592041257143714ull}},
                             V8{V16{11069648392404057882ull}, V16{2079041634138855219ull}}}}},
                     V{V2{V4{V8{V16{2728573818160840538ull},  V16{4662837316376297461ull}},
                             V8{V16{6137006355892005048ull},  V16{5800923206238252068ull}}},
                          V4{V8{V16{2231191820854670871ull},  V16{15316879499324065790ull}},
                             V8{V16{2960037215126453454ull},  V16{9782714449412099651ull}}}},
                       V2{V4{V8{V16{15481376628249688591ull}, V16{2297028890726124697ull}},
                             V8{V16{16486151782284188362ull}, V16{3600957521465764589ull}}},
                          V4{V8{V16{2275360408448083889ull},  V16{15627771031383683790ull}},
                             V8{V16{12699819593799717121ull}, V16{11569571650931815971ull}}}}}};
        }
        else if constexpr (N == 2048)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            using V32 = Z<Unsigned, V::size/32, Wrap>;
            return R{V{V2{V4{V8{V16{V32{1802111537622626447ull},  V32{10582986074225315157ull}},
                                V16{V32{16910534105743736602ull}, V32{6102494089384615204ull}}},
                             V8{V16{V32{14928497648769587890ull}, V32{13470919978804123920ull}},
                                V16{V32{6514566595823093335ull},  V32{4017744457065615249ull}}}},
                          V4{V8{V16{V32{13072010069123558035ull}, V32{2038191827176919930ull}},
                                V16{V32{2211708501220402624ull},  V32{14842705249786753553ull}}},
                             V8{V16{V32{6838638034019201342ull},  V32{11376516843958537572ull}},
                                V16{V32{10644302461241957051ull}, V32{14414692282987623051ull}}}}},
                       V2{V4{V8{V16{V32{7842934667123713456ull},  V32{5925795406120963010ull}},
                                V16{V32{11008213935837437111ull}, V32{7634434386041557576ull}}},
                             V8{V16{V32{10341715108704822369ull}, V32{12613841470879923672ull}},
                                V16{V32{3028581978962742864ull},  V32{8453284858816725906ull}}}},
                          V4{V8{V16{V32{907518820148855635ull},   V32{2849898964066225443ull}},
                                V16{V32{15449472924850233672ull}, V32{13469180397576869185ull}}},
                             V8{V16{V32{17476301469983149758ull}, V32{1068262973987646746ull}},
                                V16{V32{881625593701647129ull},   V32{11929692689860701907ull}}}}}},
                     V{V2{V4{V8{V16{V32{782647096563691066ull},   V32{16504659026601236497ull}},
                                V16{V32{7864449943562090978ull},  V32{16939175189355221651ull}}},
                             V8{V16{V32{3048697131164003201ull},  V32{260889299604499171ull}},
                                V16{V32{2482685631633303057ull},  V32{14020259198114910723ull}}}},
                          V4{V8{V16{V32{63448829459478008ull},    V32{5705301109411412964ull}},
                                V16{V32{14239050086931466438ull}, V32{13964909239999763423ull}}},
                             V8{V16{V32{3966931863575571072ull},  V32{7683248078739327994ull}},
                                V16{V32{11566697110469630574ull}, V32{5665721882472824060ull}}}}},
                       V2{V4{V8{V16{V32{17919759627221304332ull}, V32{13260008052167355569ull}},
                                V16{V32{9736689424561228611ull},  V32{12743692744907218376ull}}},
                             V8{V16{V32{16982099024137349300ull}, V32{4607733183610619506ull}},
                                V16{V32{3210848046271137756ull},  V32{2340514518543757793ull}}}},
                          V4{V8{V16{V32{15326027393100893748ull}, V32{9048469726063230369ull}},
                                V16{V32{1082361831117611165ull},  V32{18395585455466730588ull}}},
                             V8{V16{V32{16112741638006503104ull}, V32{11570589630072917061ull}},
                                V16{V32{6349792492480366466ull},  V32{8359288869691149744ull}}}}}}};
        }
        else if constexpr (N == 4096)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            using V32 = Z<Unsigned, V::size/32, Wrap>;
            using V64 = Z<Unsigned, V::size/64, Wrap>;
            return R{V{V2{V4{V8{V16{V32{V64{7149531585495722338ull},  V64{3356743370330007047ull}},
                                    V32{V64{18436378944728638077ull}, V64{7931647810978445383ull}}},
                                V16{V32{V64{3270286543224301369ull},  V64{8535398487464646401ull}},
                                    V32{V64{13484619641182843465ull}, V64{11708098033230098031ull}}}},
                             V8{V16{V32{V64{4466755116536013457ull},  V64{9656053791203264047ull}},
                                    V32{V64{5356941947428981317ull},  V64{15580331599927391401ull}}},
                                V16{V32{V64{9595217689852186297ull},  V64{5555463656331172876ull}},
                                    V32{V64{13140824531358607237ull}, V64{11483754055083081623ull}}}}},
                          V4{V8{V16{V32{V64{6650915271330035109ull},  V64{6191017237616694713ull}},
                                    V32{V64{10659682653565942587ull}, V64{9363698483568340515ull}}},
                                V16{V32{V64{5460721686566555082ull},  V64{13115907944142941659ull}},
                                    V32{V64{2287512675261844701ull},  V64{6028892126301056191ull}}}},
                             V8{V16{V32{V64{13296661554427914428ull}, V64{12228861153954590610ull}},
                                    V32{V64{18303449200529156626ull}, V64{8065266938269972425ull}}},
                                V16{V32{V64{7716663100706491393ull},  V64{14477956932119204466ull}},
                                    V32{V64{6487656601848752456ull},  V64{9464630094968132301ull}}}}}},
                       V2{V4{V8{V16{V32{V64{6728998455498865082ull},  V64{8108165390535344294ull}},
                                    V32{V64{1656691844289434357ull},  V64{1407686820843768870ull}}},
                                V16{V32{V64{11491907928342073579ull}, V64{11985166785598890692ull}},
                                    V32{V64{11347768682761416408ull}, V64{13019738741921411125ull}}}},
                             V8{V16{V32{V64{14281505481023757595ull}, V64{655042493957754827ull}},
                                    V32{V64{6400389582117751316ull},  V64{1108415771105028284ull}}},
                                V16{V32{V64{1339476019737865544ull},  V64{5852278011722911466ull}},
                                    V32{V64{11885386312227840450ull}, V64{11306682798967501777ull}}}}},
                          V4{V8{V16{V32{V64{5609385270965558020ull},  V64{8539573681267016216ull}},
                                    V32{V64{16952052630168809402ull}, V64{16066667031233871179ull}}},
                                V16{V32{V64{11879931294852716627ull}, V64{8989744946091857411ull}},
                                    V32{V64{5856138847350594593ull},  V64{11546337876978589818ull}}}},
                             V8{V16{V32{V64{7733458901375962947ull},  V64{13192023582609359432ull}},
                                    V32{V64{3751030629222208995ull},  V64{755999078468850996ull}}},
                                V16{V32{V64{3413814586276591454ull},  V64{6233140361321802513ull}},
                                    V32{V64{10257263490331005032ull}, V64{9900616889637376675ull}}}}}}},
                     V{V2{V4{V8{V16{V32{V64{3105002115773799332ull},  V64{541035047115361761ull}},
                                    V32{V64{14943693004821214509ull}, V64{16344971151659578288ull}}},
                                V16{V32{V64{16898372803676398985ull}, V64{8545022163871786795ull}},
                                    V32{V64{2417044535915079116ull},  V64{4094041142132511454ull}}}},
                             V8{V16{V32{V64{398777859991749938ull},   V64{14472023099744176931ull}},
                                    V32{V64{1569487742415141348ull},  V64{5667739480594692884ull}}},
                                V16{V32{V64{16618279199955746803ull}, V64{6045972684290830226ull}},
                                    V32{V64{10621838157148386528ull}, V64{4300389138668186743ull}}}}},
                          V4{V8{V16{V32{V64{15150039749540264412ull}, V64{17299598480090105958ull}},
                                    V32{V64{4542601629997426411ull},  V64{5322714947163197625ull}}},
                                V16{V32{V64{10998594939406384295ull}, V64{7195419241306291014ull}},
                                    V32{V64{7169174092594494727ull},  V64{12739833686425767279ull}}}},
                             V8{V16{V32{V64{14281315903261526099ull}, V64{16741491564656035938ull}},
                                    V32{V64{6119278703406663144ull},  V64{10294772841165069088ull}}},
                                V16{V32{V64{5923124271538059325ull},  V64{12744397924891507191ull}},
                                    V32{V64{10095554246618937786ull}, V64{16365726348988042767ull}}}}}},
                       V2{V4{V8{V16{V32{V64{14945548739992358943ull}, V64{14389139788888140367ull}},
                                    V32{V64{16579448618610455943ull}, V64{8375877691829025699ull}}},
                                V16{V32{V64{10029308622813594987ull}, V64{4606628300370984785ull}},
                                    V32{V64{4781633361422667784ull},  V64{2793620847725421482ull}}}},
                             V8{V16{V32{V64{13722788574240689077ull}, V64{12287402620953490367ull}},
                                    V32{V64{1334123232330176541ull},  V64{13045152750333873732ull}}},
                                V16{V32{V64{9859709127418628314ull},  V64{4849739148367919730ull}},
                                    V32{V64{13798964963182522441ull}, V64{561229047326533081ull}}}}},
                          V4{V8{V16{V32{V64{18072840013260851745ull}, V64{1152556210591610368ull}},
                                    V32{V64{14893877886133359895ull}, V64{5144311395223912405ull}}},
                                V16{V32{V64{183288990521341218ull},   V64{8054685708400689537ull}},
                                    V32{V64{10935414408000067955ull}, V64{12052376713306787751ull}}}},
                             V8{V16{V32{V64{10840656168174498900ull}, V64{12885463317033900174ull}},
                                    V32{V64{16683667412638369334ull}, V64{5627124173670586750ull}}},
                                V16{V32{V64{860080777293011832ull},   V64{10066081668216524616ull}},
                                    V32{V64{10726523864997694263ull}, V64{5385553782061620983ull}}}}}}}};
        }
        else
        {
            throw "higher bit widths for ln10 not supported";
        }
    }();

template <unsigned N>
constexpr
auto log10e = []()
    {
        return reciprocal(ln10<N>);
    }();

template <unsigned N>
constexpr
auto inv_sqrtpi = []()
    {
        using R = rational<N>;
        using V = typename R::value_type;
        if constexpr (N == 8)
        {
            return R{V{22}, V{39}};
        }
        else if constexpr (N == 16)
        {
            return R{V{4821}, V{8545}};
        }
        else if constexpr (N == 32)
        {
            return R{V{1196609286}, V{2120934737}};
        }
        else if constexpr (N == 64)
        {
            return R{V{4323360795912412649}, V{7662957491568892555}};
        }
        else if constexpr (N == 128)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            return R{V{V2{1623753980598883548ull}, V2{5724660887849765850ull}},
                     V{V2{2878028995835651704ull}, V2{13388648846686929543ull}}};
        }
        else if constexpr (N == 256)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            return R{V{V2{V4{2279971998673897545ull}, V4{10162225948353142007ull}},
                       V2{V4{7454287535221673572ull}, V4{10957122244896545339ull}}},
                     V{V2{V4{4041145149006295785ull}, V4{13050801389593008259ull}},
                       V2{V4{946326000166797637ull},  V4{15008027024304879943ull}}}};
        }
        else if constexpr (N == 512)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            return R{V{V2{V4{V8{4802324968813674300ull},  V8{6928170404792426487ull}},
                          V4{V8{11124798319726825841ull}, V8{1544313245727054140ull}}},
                       V2{V4{V8{5262103370872311070ull},  V8{5187397650530369492ull}},
                          V4{V8{10806189697371065062ull}, V8{11178165317287070850ull}}}},
                     V{V2{V4{V8{8511899384273509173ull},  V8{16633467416790552703ull}},
                          V4{V8{278761419382229627ull},   V8{4887520775117785325ull}}},
                       V2{V4{V8{3601276567595219853ull},  V8{15707075327666397563ull}},
                          V4{V8{8138032949357334542ull},  V8{6692333513251756235ull}}}}};
        }
        else if constexpr (N == 1024)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            return R{V{V2{V4{V8{V16{2955326503940056971ull},  V16{1611706571071568609ull}},
                             V8{V16{5213570967158196962ull},  V16{382736314995213141ull}}},
                          V4{V8{V16{14433735877661830928ull}, V16{3241624988514205532ull}},
                             V8{V16{17810768696211494205ull}, V16{17037506084573383038ull}}}},
                       V2{V4{V8{V16{1128135454055524168ull},  V16{9446608171409195416ull}},
                             V8{V16{16343857957815973841ull}, V16{5379912494147656086ull}}},
                          V4{V8{V16{13755424676417285810ull}, V16{8470431506282043931ull}},
                             V8{V16{15630184125043843892ull}, V16{16789235679459451326ull}}}}},
                     V{V2{V4{V8{V16{5238179842591689662ull},  V16{15494002172996959754ull}},
                             V8{V16{7291091354608628818ull},  V16{14761355359386069712ull}}},
                          V4{V8{V16{1603426422851485115ull},  V16{15070482353327620992ull}},
                             V8{V16{7284458230772441059ull},  V16{697057374065082820ull}}}},
                       V2{V4{V8{V16{17430750846404529723ull}, V16{4946815298430940387ull}},
                             V8{V16{18241339857485708794ull}, V16{16038672165961649334ull}}},
                          V4{V8{V16{2359324142482114553ull},  V16{1776613042635155896ull}},
                             V8{V16{5051788897870565822ull},  V16{11456671985755169113ull}}}}}};
        }
        else if constexpr (N == 2048)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            using V32 = Z<Unsigned, V::size/32, Wrap>;
            return R{V{V2{V4{V8{V16{V32{2802219617512605602ull},  V32{16701235294483073620ull}},
                                V16{V32{17385225478247993960ull}, V32{8360443398537367279ull}}},
                             V8{V16{V32{14374843610636727380ull}, V32{11761555226296537235ull}},
                                V16{V32{9986357875562414878ull},  V32{7700083498746298359ull}}}},
                          V4{V8{V16{V32{10418830343471395115ull}, V32{16797468380549836032ull}},
                                V16{V32{15211073686340800268ull}, V32{2953386704422364028ull}}},
                             V8{V16{V32{14695851938366536513ull}, V32{1068720023255672852ull}},
                                V16{V32{3975746077562336989ull},  V32{15618039436677380641ull}}}}},
                       V2{V4{V8{V16{V32{900071212749357515ull},   V32{15076317188713961945ull}},
                                V16{V32{2170797971328464420ull},  V32{12963062923739060066ull}}},
                             V8{V16{V32{1530337790462828157ull},  V32{16276661620909811580ull}},
                                V16{V32{5105951207863633349ull},  V32{14473283927258886120ull}}}},
                          V4{V8{V16{V32{8467888192508422858ull},  V32{16854936645864304885ull}},
                                V16{V32{11539094691846504373ull}, V32{8077577172131536989ull}}},
                             V8{V16{V32{5127811755321720223ull},  V32{4760444488726213580ull}},
                                V16{V32{5751000046631279703ull},  V32{16511308543209136617ull}}}}}},
                     V{V2{V4{V8{V16{V32{4966804952143200000ull},  V32{1258243284127255666ull}},
                                V16{V32{3869340509263542366ull},  V32{5137931419806323698ull}}},
                             V8{V16{V32{8574564637067583475ull},  V32{14856002592648981626ull}},
                                V16{V32{7482430057182120846ull},  V32{562864561763050208ull}}}},
                          V4{V8{V16{V32{14313709212343008603ull}, V32{16578924992832145747ull}},
                                V16{V32{8374129521689903886ull},  V32{11891432961404710519ull}}},
                             V8{V16{V32{11106243121512394915ull}, V32{6865949037899818053ull}},
                                V16{V32{11123561859585861063ull}, V32{8428605773868653234ull}}}}},
                       V2{V4{V8{V16{V32{4506875904625719809ull},  V32{8472885476151176223ull}},
                                V16{V32{3482826736228931490ull},  V32{7286480945880683795ull}}},
                             V8{V16{V32{7058077888829788042ull},  V32{17957923411706677455ull}},
                                V16{V32{8917765357539377878ull},  V32{3266606086315458864ull}}}},
                          V4{V8{V16{V32{16684205371466022862ull}, V32{7209452000744381535ull}},
                                V16{V32{10120295986883285103ull}, V32{6864140725128608225ull}}},
                             V8{V16{V32{6331881474113548929ull},  V32{7583556446534894296ull}},
                                V16{V32{4806767437037017165ull},  V32{12002813327713274546ull}}}}}}};
        }
        else if constexpr (N == 4096)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            using V32 = Z<Unsigned, V::size/32, Wrap>;
            using V64 = Z<Unsigned, V::size/64, Wrap>;
            return R{V{V2{V4{V8{V16{V32{V64{181829562884392697ull},   V64{6643770010306872880ull}},
                                    V32{V64{12031617665402461954ull}, V64{18056289967566335389ull}}},
                                V16{V32{V64{2510328727094771867ull},  V64{3203843079429622985ull}},
                                    V32{V64{14109789893491814704ull}, V64{3382261363627914377ull}}}},
                             V8{V16{V32{V64{8698947416533000367ull},  V64{9240774378568251309ull}},
                                    V32{V64{18363483618151646448ull}, V64{17891615957753528490ull}}},
                                V16{V32{V64{8365825402236664856ull},  V64{17539142875556683971ull}},
                                    V32{V64{13776087602803529773ull}, V64{10675737938046011658ull}}}}},
                          V4{V8{V16{V32{V64{12212242328838132143ull}, V64{8770173181637685471ull}},
                                    V32{V64{8522031080432367257ull},  V64{880791560604222838ull}}},
                                V16{V32{V64{3271310979585003831ull},  V64{3768685589769386638ull}},
                                    V32{V64{7332990344771935931ull},  V64{5763394317398170432ull}}}},
                             V8{V16{V32{V64{17082415167928574240ull}, V64{1584429835755494097ull}},
                                    V32{V64{17077962366403070801ull}, V64{8536347035490366874ull}}},
                                V16{V32{V64{48540070547922995ull},    V64{17002530636243619507ull}},
                                    V32{V64{7726554456237039089ull},  V64{1770339021781108368ull}}}}}},
                       V2{V4{V8{V16{V32{V64{17972012585654506076ull}, V64{5321553543953699697ull}},
                                    V32{V64{6060445237149211186ull},  V64{10196562640811666308ull}}},
                                V16{V32{V64{12079127056274559798ull}, V64{8906597228832127745ull}},
                                    V32{V64{3714720257078737778ull},  V64{6845909444942114280ull}}}},
                             V8{V16{V32{V64{18258121000261168419ull}, V64{12154150501707344888ull}},
                                    V32{V64{13926341354424810648ull}, V64{4722599466301158795ull}}},
                                V16{V32{V64{9301894114805911061ull},  V64{6957164295323083464ull}},
                                    V32{V64{11684070536842069245ull}, V64{12120780379632784981ull}}}}},
                          V4{V8{V16{V32{V64{15896204319334991319ull}, V64{11931618453864650778ull}},
                                    V32{V64{4171945179973111901ull},  V64{1456502791132885051ull}}},
                                V16{V32{V64{14196330781661924931ull}, V64{16429224843742005778ull}},
                                    V32{V64{200844421329945531ull},   V64{17824271278903851455ull}}}},
                             V8{V16{V32{V64{17540336029770843309ull}, V64{6279328056511404104ull}},
                                    V32{V64{4281360527371751889ull},  V64{10759425427583531166ull}}},
                                V16{V32{V64{1835437038233689223ull},  V64{6536201531866611868ull}},
                                    V32{V64{15877746517716657119ull}, V64{6900132873601422717ull}}}}}}},
                     V{V2{V4{V8{V16{V32{V64{322284508942908524ull},   V64{14320892359776149205ull}},
                                    V32{V64{4782280882772278777ull},  V64{2794345134693647972ull}}},
                                V16{V32{V64{1723661726597190119ull},  V64{14917060313497806731ull}},
                                    V32{V64{16142732802804406407ull}, V64{5444087543241684882ull}}}},
                             V8{V16{V32{V64{16028096984138937107ull}, V64{3011430350482809477ull}},
                                    V32{V64{10009424478719369276ull}, V64{15782563423395226471ull}}},
                                V16{V32{V64{6521409597212559231ull},  V64{4052477978723840569ull}},
                                    V32{V64{17420424059318551591ull}, V64{3169747386742965197ull}}}}},
                          V4{V8{V16{V32{V64{11143242149778757942ull}, V64{10620588629566326554ull}},
                                    V32{V64{4559718244475751903ull},  V64{10123283203531714953ull}}},
                                V16{V32{V64{7773949993821431090ull},  V64{16630419821246871284ull}},
                                    V32{V64{6382726976206454436ull},  V64{4485682207236458202ull}}}},
                             V8{V16{V32{V64{2377908077708024255ull},  V64{4549239180120601268ull}},
                                    V32{V64{4744895429630118915ull},  V64{13974867506658525418ull}}},
                                V16{V32{V64{16107390566813217354ull}, V64{1947339305309407048ull}},
                                    V32{V64{12143874283126934228ull}, V64{1821402239944632563ull}}}}}},
                       V2{V4{V8{V16{V32{V64{4000651820794112327ull},  V64{12524284978104200191ull}},
                                    V32{V64{12505911632815746219ull}, V64{6010173572537401657ull}}},
                                V16{V32{V64{18011525492154897812ull}, V64{8566231300828778593ull}},
                                    V32{V64{6328318463717602378ull},  V64{12606299862914591239ull}}}},
                             V8{V16{V32{V64{11974262909813342718ull}, V64{13199551563155820413ull}},
                                    V32{V64{3528635015651210430ull},  V64{1633010256532872732ull}}},
                                V16{V32{V64{5438453752123205904ull},  V64{10603702826590765151ull}},
                                    V32{V64{13462324244858886802ull}, V64{12407504907389560273ull}}}}},
                          V4{V8{V16{V32{V64{3545835578950224931ull},  V64{4577440984366302088ull}},
                                    V32{V64{17207303476550989154ull}, V64{10247053080234171031ull}}},
                                V16{V32{V64{5269988624629698395ull},  V64{16772991575177796966ull}},
                                    V32{V64{11088900518540630151ull}, V64{11153302936782070384ull}}}},
                             V8{V16{V32{V64{5402235373136083547ull},  V64{7765140768170586252ull}},
                                    V32{V64{4740665526967473690ull},  V64{5381233136125268577ull}}},
                                V16{V32{V64{11826454365662520919ull}, V64{8455692850029406661ull}},
                                    V32{V64{2594510210492792353ull},  V64{4106232269719228477ull}}}}}}}};
        }
        else
        {
            throw "higher bit widths for inv_sqrtpi not supported";
        }
    }();

template <unsigned N>
constexpr
auto egamma = []()
    {
        using R = rational<N>;
        using V = typename R::value_type;
        if constexpr (N == 8)
        {
            return R{V{71}, V{123}};
        }
        else if constexpr (N == 16)
        {
            return R{V{18438}, V{31943}};
        }
        else if constexpr (N == 32)
        {
            return R{V{376566901}, V{652385103}};
        }
        else if constexpr (N == 64)
        {
            return R{V{4434255124552851345}, V{7682146196273606513}};
        }
        else if constexpr (N == 128)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            return R{V{V2{5269913864156369823ull}, V2{13614572022867069336ull}},
                     V{V2{9129887119496945203ull}, V2{17965527286813347045ull}}};
        }
        else if constexpr (N == 256)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            return R{V{V2{V4{542514727311240931ull},   V4{483629540911284732ull}},
                       V2{V4{15089855547781706707ull}, V4{16483807852328062898ull}}},
                     V{V2{V4{939882197070636401ull},   V4{5183866545656922512ull}},
                       V2{V4{3588081914117373725ull},  V4{11206558323763108373ull}}}};
        }
        else if constexpr (N == 512)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            return R{V{V2{V4{V8{1359297313067839327ull},  V8{5215851799124931589ull}},
                          V4{V8{15995193690549088487ull}, V8{3175050796735457715ull}}},
                       V2{V4{V8{7042616975120972013ull},  V8{16191156138819469544ull}},
                          V4{V8{1421511776031068281ull},  V8{9738642603779406769ull}}}},
                     V{V2{V4{V8{2354921038568351511ull},  V8{2316942272658430993ull}},
                          V4{V8{3236176578837604834ull},  V8{9068774526088683911ull}}},
                       V2{V4{V8{12943486317117273960ull}, V8{14627796954058728590ull}},
                          V4{V8{2009029668436223059ull},  V8{5353425007098782045ull}}}}};
        }
        else if constexpr (N == 1024)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            return R{V{V2{V4{V8{V16{1824134583290014289ull},  V16{14485084499125612433ull}},
                             V8{V16{11250424200829480510ull}, V16{10367081153928132012ull}}},
                          V4{V8{V16{10836464379722368027ull}, V16{18239999471632770912ull}},
                             V8{V16{14671943286458497457ull}, V16{6107664770700048116ull}}}},
                       V2{V4{V8{V16{2592025783961122191ull},  V16{13440830225341003365ull}},
                             V8{V16{7506292099920993162ull},  V16{3779405998197326772ull}}},
                          V4{V8{V16{14733400465345460163ull}, V16{16394825472310099360ull}},
                             V8{V16{1152681841169628454ull},  V16{15828416047587672684ull}}}}},
                     V{V2{V4{V8{V16{3160230558886847176ull},  V16{9411590042933978310ull}},
                             V8{V16{10715482353005113585ull}, V16{14655087633663406630ull}}},
                          V4{V8{V16{4932175228601294393ull},  V16{1010288963128981894ull}},
                             V8{V16{2604387949971676650ull},  V16{16204723450621095694ull}}}},
                       V2{V4{V8{V16{3425215261247584111ull},  V16{17606516657734050825ull}},
                             V8{V16{5341287740457235622ull},  V16{12736126746662157286ull}}},
                          V4{V8{V16{8806429479691856624ull},  V16{2864612415598949010ull}},
                             V8{V16{13610460835443830204ull}, V16{1953863686068255941ull}}}}}};
        }
        else if constexpr (N == 2048)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            using V32 = Z<Unsigned, V::size/32, Wrap>;
            return R{V{V2{V4{V8{V16{V32{4615610443740954663ull},  V32{16216974589934383791ull}},
                                V16{V32{18009522273765177975ull}, V32{2694388148729445802ull}}},
                             V8{V16{V32{18172566334503961151ull}, V32{8081831624759421325ull}},
                                V16{V32{11855921629413141734ull}, V32{1386751452454830314ull}}}},
                          V4{V8{V16{V32{16127197217400395665ull}, V32{9810970258763890769ull}},
                                V16{V32{12885707344175819910ull}, V32{8415740504908421200ull}}},
                             V8{V16{V32{9542445759014497190ull},  V32{9545220334552159145ull}},
                                V16{V32{11193809430260117519ull}, V32{14329645304603346904ull}}}}},
                       V2{V4{V8{V16{V32{11387507225492580002ull}, V32{18246848938636983580ull}},
                                V16{V32{18312087309435219897ull}, V32{10836525248817376859ull}}},
                             V8{V16{V32{15608118819563162434ull}, V32{6092865313238640974ull}},
                                V16{V32{1937730471853393398ull},  V32{14482494675793874736ull}}}},
                          V4{V8{V16{V32{2312235574529879906ull},  V32{16818128643781112516ull}},
                                V16{V32{14337883169600605369ull}, V32{12289403279472900132ull}}},
                             V8{V16{V32{1386107901156288023ull},  V32{3285936006760804005ull}},
                                V16{V32{15067428061203205955ull}, V32{10351454253332633549ull}}}}}},
                     V{V2{V4{V8{V16{V32{7996336074018938835ull},  V32{15271916813960336937ull}},
                                V16{V32{14565597795789467641ull}, V32{4937870602362153356ull}}},
                             V8{V16{V32{10535739508222187131ull}, V32{14720050667259193091ull}},
                                V16{V32{7072044569372358484ull},  V32{6556557929600149732ull}}}},
                          V4{V8{V16{V32{8188665081764055433ull},  V32{15629854700621434576ull}},
                                V16{V32{4009918424346702353ull},  V32{8423007573765323758ull}}},
                             V8{V16{V32{15038902101400587943ull}, V32{14066690888595709593ull}},
                                V16{V32{12156445740939901784ull}, V32{3025983419931022045ull}}}}},
                       V2{V4{V8{V16{V32{6656965825199231274ull},  V32{13205047032739369346ull}},
                                V16{V32{6101248612133387546ull},  V32{9206307595556066184ull}}},
                             V8{V16{V32{9349133908103588470ull},  V32{3865627052210865855ull}},
                                V16{V32{1718651908284101657ull},  V32{15245265913551039696ull}}}},
                          V4{V8{V16{V32{2451087810301167696ull},  V32{13203555246834930020ull}},
                                V16{V32{9203131356636994635ull},  V32{82883025494230941ull}}},
                             V8{V16{V32{6190208371047991990ull},  V32{2164066601539798427ull}},
                                V16{V32{11331811518533209237ull}, V32{16763610466377538982ull}}}}}}};
        }
        else if constexpr (N == 4096)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            using V32 = Z<Unsigned, V::size/32, Wrap>;
            using V64 = Z<Unsigned, V::size/64, Wrap>;
            return R{V{V2{V4{V8{V16{V32{V64{38211852345581483ull},    V64{2499312741620917638ull}},
                                    V32{V64{10034017525899117257ull}, V64{12289424349704849283ull}}},
                                V16{V32{V64{7685990878143351267ull},  V64{18031469369780319575ull}},
                                    V32{V64{17262203462027213877ull}, V64{4252622898711364475ull}}}},
                             V8{V16{V32{V64{4180159428439084215ull},  V64{4615916989340137455ull}},
                                    V32{V64{4294265769620677155ull},  V64{17912217199823789558ull}}},
                                V16{V32{V64{8218471505194435009ull},  V64{10701466072467681275ull}},
                                    V32{V64{4298542249725593283ull},  V64{3260593495912048435ull}}}}},
                          V4{V8{V16{V32{V64{16862264607496346435ull}, V64{13558038206037323287ull}},
                                    V32{V64{10347970875437420731ull}, V64{4632185050220179986ull}}},
                                V16{V32{V64{11707652653140262937ull}, V64{13923769879101667940ull}},
                                    V32{V64{2659828504477397195ull},  V64{12596439294381892632ull}}}},
                             V8{V16{V32{V64{4198796839547616273ull},  V64{1738022738971412979ull}},
                                    V32{V64{15213898407131499726ull}, V64{7573857183522077037ull}}},
                                V16{V32{V64{15547336325351602057ull}, V64{13832112058803911268ull}},
                                    V32{V64{8016101450510348043ull},  V64{2136554777459042450ull}}}}}},
                       V2{V4{V8{V16{V32{V64{13687256696321871546ull}, V64{13805698439759835268ull}},
                                    V32{V64{15047747270638033507ull}, V64{17068410865667548252ull}}},
                                V16{V32{V64{16664069024546711150ull}, V64{2007455185624086579ull}},
                                    V32{V64{1845323961062577243ull},  V64{9230850152436435985ull}}}},
                             V8{V16{V32{V64{9100834064344821161ull},  V64{13555493477289690467ull}},
                                    V32{V64{10864333833584239749ull}, V64{2147740779250402069ull}}},
                                V16{V32{V64{1384256650227805902ull},  V64{8726670709508886443ull}},
                                    V32{V64{6417615776635826608ull},  V64{7406435196397248807ull}}}}},
                          V4{V8{V16{V32{V64{16415110391830705584ull}, V64{6463715541471592610ull}},
                                    V32{V64{17912448998813928343ull}, V64{14228034271049947392ull}}},
                                V16{V32{V64{14346986325321906638ull}, V64{17544196840925242266ull}},
                                    V32{V64{9993403210392790502ull},  V64{5821533034627198354ull}}}},
                             V8{V16{V32{V64{7105305912342938714ull},  V64{4734034988226458437ull}},
                                    V32{V64{6918668042869765914ull},  V64{13095068108746554881ull}}},
                                V16{V32{V64{4731792077897853023ull},  V64{13388421098974375908ull}},
                                    V32{V64{2436984228075886543ull},  V64{16834434480983896777ull}}}}}}},
                     V{V2{V4{V8{V16{V32{V64{66200303749725915ull},    V64{2500566698417046816ull}},
                                    V32{V64{9732750774876369445ull},  V64{2571277020588185829ull}}},
                                V16{V32{V64{14174539577445081039ull}, V64{14525858320937430443ull}},
                                    V32{V64{7674164845349966781ull},  V64{14834984838269137737ull}}}},
                             V8{V16{V32{V64{8045375932828890701ull},  V64{5538425523007210755ull}},
                                    V32{V64{159175413556447890ull},   V64{18053838912778877934ull}}},
                                V16{V32{V64{5166116053626642917ull},  V64{2246081773508273924ull}},
                                    V32{V64{4795492601357010517ull},  V64{10401216687452240168ull}}}}},
                          V4{V8{V16{V32{V64{8521221499616080617ull},  V64{5331937077898287229ull}},
                                    V32{V64{12506546905194996358ull}, V64{11715065088856523854ull}}},
                                V16{V32{V64{14324032421308495795ull}, V64{1980448207895678889ull}},
                                    V32{V64{7409505226683589720ull},  V64{4626628635004731471ull}}}},
                             V8{V16{V32{V64{8094978135900963910ull},  V64{1623201868857371574ull}},
                                    V32{V64{6522338727190807982ull},  V64{6736724745721119879ull}}},
                                V16{V32{V64{15603906506106470997ull}, V64{4502999477668305646ull}},
                                    V32{V64{11154552826809985889ull}, V64{1973846366053323697ull}}}}}},
                       V2{V4{V8{V16{V32{V64{7173936733926486829ull},  V64{5877528775924204273ull}},
                                    V32{V64{18271967589889838931ull}, V64{15017518184561513610ull}}},
                                V16{V32{V64{11743793532287952382ull}, V64{13326353804457640131ull}},
                                    V32{V64{8916647795499946138ull},  V64{3720107422874755970ull}}}},
                             V8{V16{V32{V64{16558841560367760577ull}, V64{6145207035979062940ull}},
                                    V32{V64{15540256120855328193ull}, V64{7723702418988528282ull}}},
                                V16{V32{V64{7726495556772665841ull},  V64{7818947114751332865ull}},
                                    V32{V64{5409827611409071348ull},  V64{1782718842315482041ull}}}}},
                          V4{V8{V16{V32{V64{3728430384882277600ull},  V64{2195729063591702410ull}},
                                    V32{V64{9503993577211979550ull},  V64{515677118776814688ull}}},
                                V16{V32{V64{7248146900171759615ull},  V64{4259181538001348862ull}},
                                    V32{V64{15778613978569526462ull}, V64{6215142412881468014ull}}}},
                             V8{V16{V32{V64{8460442771745306205ull},  V64{18445863524574968851ull}},
                                    V32{V64{17978938698932465723ull}, V64{11002198920796896089ull}}},
                                V16{V32{V64{524197894203958943ull},   V64{13998444584587082409ull}},
                                    V32{V64{14961989231973030144ull}, V64{6521635752536138395ull}}}}}}}};
        }
        else
        {
            throw "higher bit widths for egamma not supported";
        }
    }();

template <unsigned N>
constexpr
auto phi = []()
    {
        using R = rational<N>;
        using V = typename R::value_type;
        if constexpr (N == 8)
        {
            return R{V{89}, V{55}};
        }
        else if constexpr (N == 16)
        {
            return R{V{28657}, V{17711}};
        }
        else if constexpr (N == 32)
        {
            return R{V{1836311903}, V{1134903170}};
        }
        else if constexpr (N == 64)
        {
            return R{V{7540113804746346429}, V{4660046610375530309}};
        }
        else if constexpr (N == 128)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            return R{V{V2{6891616170087056899ull}, V2{10993266775918486379ull}},
                     V{V2{4259253030532177544ull}, V2{10918772188714576418ull}}};
        }
        else if constexpr (N == 256)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            return R{V{V2{V4{5757148639720064485ull},  V4{5327951006943957288ull}},
                       V2{V4{9165097436049010405ull},  V4{18019343248380654373ull}}},
                     V{V2{V4{3558113537632222762ull},  V4{13546247192960911575ull}},
                       V2{V4{16387055819095874687ull}, V4{15586334566667950909ull}}}};
        }
        else if constexpr (N == 512)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            return R{V{V2{V4{V8{6500814962980377907ull},  V8{13935479701484769272ull}},
                          V4{V8{6526560820842198291ull},  V8{13641217818503742230ull}}},
                       V2{V4{V8{15451103999744565165ull}, V8{17325965721145598031ull}},
                          V4{V8{822412014878677776ull},   V8{1230418563085246685ull}}}},
                     V{V2{V4{V8{4017724601695762973ull},  V8{17477743522418975050ull}},
                          V4{V8{6084106054633720492ull},  V8{1234526015038827834ull}}},
                       V2{V4{V8{2954677974016290953ull},  V8{11378543557876294251ull}},
                          V4{V8{12945902499222113505ull}, V8{7415982149307855099ull}}}}};
        }
        else if constexpr (N == 1024)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            return R{V{V2{V4{V8{V16{8288740232184475782ull},  V16{13208642241852822377ull}},
                             V8{V16{16461748489962553240ull}, V16{11298899283853474204ull}}},
                          V4{V8{V16{734157270866138484ull},   V16{8399253158222984549ull}},
                             V8{V16{12238159830917017265ull}, V16{1690627509696382029ull}}}},
                       V2{V4{V8{V16{1880407357377364335ull},  V16{15478919584381105889ull}},
                             V8{V16{14401030798031862193ull}, V16{1694230293706796909ull}}},
                          V4{V8{V16{3862781344314794610ull},  V16{3672678793545941582ull}},
                             V8{V16{7478080055284804294ull},  V16{7150927418486842633ull}}}}},
                     V{V2{V4{V8{V16{5122723187408701117ull},  V16{16650882813929469863ull}},
                             V8{V16{6102023347731260089ull},  V16{15929599236674606626ull}}},
                          V4{V8{V16{2497876328445603305ull},  V16{4427413469305395149ull}},
                             V8{V16{9619364993650612351ull},  V16{4188865323717723431ull}}}},
                       V2{V4{V8{V16{1898062679621024117ull},  V16{17606763520425603524ull}},
                             V8{V16{4040218890177311047ull},  V16{8143604885650769853ull}}},
                          V4{V8{V16{4018372065275378183ull},  V16{1472505960603099028ull}},
                             V8{V16{16877987869564032096ull}, V16{6190977578781691943ull}}}}}};
        }
        else if constexpr (N == 2048)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            using V32 = Z<Unsigned, V::size/32, Wrap>;
            return R{V{V2{V4{V8{V16{V32{8328031092447301818ull},  V32{17228224225546302025ull}},
                                V16{V32{13397555341704950811ull}, V32{7035090904524337744ull}}},
                             V8{V16{V32{15748726710232329466ull}, V32{17353760490349231829ull}},
                                V16{V32{4673522814865379308ull},  V32{66252179915471217ull}}}},
                          V4{V8{V16{V32{16754020871553670309ull}, V32{8064106371026550144ull}},
                                V16{V32{6841230381507427100ull},  V32{13041115042809227520ull}}},
                             V8{V16{V32{3386025151868004627ull},  V32{7388181247363273207ull}},
                                V16{V32{16581218204429475140ull}, V32{4195618779690967482ull}}}}},
                       V2{V4{V8{V16{V32{15151265082403468079ull}, V32{6279794310421689589ull}},
                                V16{V32{1541328542450497215ull},  V32{8393128101111622234ull}}},
                             V8{V16{V32{47245103011614651ull},    V32{16234869478758197836ull}},
                                V16{V32{17511007166314741345ull}, V32{16188727164423980238ull}}}},
                          V4{V8{V16{V32{12372862792247494256ull}, V32{9334156967862595732ull}},
                                V16{V32{14722832697613330970ull}, V32{2379907902830862624ull}}},
                             V8{V16{V32{5851993498995060997ull},  V32{17164452174989894675ull}},
                                V16{V32{987881083924103311ull},   V32{14028478700514542351ull}}}}}},
                     V{V2{V4{V8{V16{V32{5147006274498350234ull},  V32{16772667450855072457ull}},
                                V16{V32{10334360990075176471ull}, V32{3467496655565515704ull}}},
                             V8{V16{V32{10312068305188324009ull}, V32{6956485466181800303ull}},
                                V16{V32{16232626451138688039ull}, V32{874153833622963255ull}}}},
                          V4{V8{V16{V32{17120089519574182849ull}, V32{17764262332263244111ull}},
                                V16{V32{1437857539222991975ull},  V32{18325135465881340138ull}}},
                             V8{V16{V32{6803932625638775929ull},  V32{16741611462491795673ull}},
                                V16{V32{11103858260808783460ull}, V32{4845113455611911593ull}}}}},
                       V2{V4{V8{V16{V32{12787325416977592415ull}, V32{17986793624276125877ull}},
                                V16{V32{11049848560161439381ull}, V32{8796228029646883754ull}}},
                             V8{V16{V32{767173576051007906ull},   V32{14417668936630213321ull}},
                                V16{V32{2671295959251558477ull},  V32{13271777956743859490ull}}}},
                          V4{V8{V16{V32{15880571096178821032ull}, V32{2817411955696029401ull}},
                                V16{V32{10555626530496256451ull}, V32{5226399882356718192ull}}},
                             V8{V16{V32{13692208781558508471ull}, V32{14756657520360134314ull}},
                                V16{V32{1983221689272080036ull},  V32{6735289973603532866ull}}}}}}};
        }
        else if constexpr (N == 4096)
        {
            using V2 = Z<Unsigned, V::size/2, Wrap>;
            using V4 = Z<Unsigned, V::size/4, Wrap>;
            using V8 = Z<Unsigned, V::size/8, Wrap>;
            using V16 = Z<Unsigned, V::size/16, Wrap>;
            using V32 = Z<Unsigned, V::size/32, Wrap>;
            using V64 = Z<Unsigned, V::size/64, Wrap>;
            return R{V{V2{V4{V8{V16{V32{V64{8407172443612125459ull},  V64{15638363060120850162ull}},
                                    V32{V64{9717685286004084970ull},  V64{11317969121667859387ull}}},
                                V16{V32{V64{17180199106651393336ull}, V64{12196868788443974485ull}},
                                    V32{V64{6666478879511800748ull},  V64{4180628501875964876ull}}}},
                             V8{V16{V32{V64{10493916232776618296ull}, V64{4161903941618371114ull}},
                                    V32{V64{2892426793367885793ull},  V64{14314436989842314474ull}}},
                                V16{V32{V64{7742770312616992701ull},  V64{17489829522117994449ull}},
                                    V32{V64{14175119595456989210ull}, V64{16684594523985811449ull}}}}},
                          V4{V8{V16{V32{V64{7241681917624023082ull},  V64{8029443318124145258ull}},
                                    V32{V64{10246725361124466763ull}, V64{2395364882973538225ull}}},
                                V16{V32{V64{16667259411598557945ull}, V64{8317819526012584306ull}},
                                    V32{V64{1099835380620935949ull},  V64{6809863648678424994ull}}}},
                             V8{V16{V32{V64{10964436696816347029ull}, V64{10856773611038528922ull}},
                                    V32{V64{1187525420195938984ull},  V64{13753954416098668447ull}}},
                                V16{V32{V64{14054024423353033042ull}, V64{481491380911088469ull}},
                                    V32{V64{12103589435833246300ull}, V64{1794547388692004523ull}}}}}},
                       V2{V4{V8{V16{V32{V64{7241150869661416419ull},  V64{18135578421341912137ull}},
                                    V32{V64{13713273549283618399ull}, V64{6813406946389738417ull}}},
                                V16{V32{V64{12210982355398601711ull}, V64{5190269578037928041ull}},
                                    V32{V64{1081176812178508484ull},  V64{10390729447603488220ull}}}},
                             V8{V16{V32{V64{13836092264798477864ull}, V64{13263615302615573478ull}},
                                    V32{V64{12970426776101087754ull}, V64{15104907138524730507ull}}},
                                V16{V32{V64{9013461053724379279ull},  V64{12575841287230224670ull}},
                                    V32{V64{9165016720305800718ull},  V64{11183112479010101969ull}}}}},
                          V4{V8{V16{V32{V64{4458083257809639805ull},  V64{147549580615476965ull}},
                                    V32{V64{1392164879524944060ull},  V64{8838216226799449746ull}}},
                                V16{V32{V64{13451306400952111323ull}, V64{1687743551846740697ull}},
                                    V32{V64{35957332707106679ull},    V64{7282995919383033531ull}}}},
                             V8{V16{V32{V64{14200267394250749305ull}, V64{5313830910709634429ull}},
                                    V32{V64{4908411665023380400ull},  V64{11695016404375335551ull}}},
                                V16{V32{V64{15621493214394215141ull}, V64{15683035742305894001ull}},
                                    V32{V64{17124404102522922941ull}, V64{9688251562169659037ull}}}}}}},
                     V{V2{V4{V8{V16{V32{V64{5195918319433802326ull},  V64{9930685562409968842ull}},
                                    V32{V64{8683544295063302659ull},  V64{16878877956023469378ull}}},
                                V16{V32{V64{7622537450109824757ull},  V64{18407582236963501186ull}},
                                    V32{V64{8030176207124738248ull},  V64{4999440854898930077ull}}}},
                             V8{V16{V32{V64{16893192810375849942ull}, V64{11557876667174043929ull}},
                                    V32{V64{15573803669595534974ull}, V64{811333766048126130ull}}},
                                V16{V32{V64{4002314838339485742ull},  V64{5273003126585613735ull}},
                                    V32{V64{13458740860497811272ull}, V64{11337171999870352632ull}}}}},
                          V4{V8{V16{V32{V64{17937105914942732607ull}, V64{5409253757089732338ull}},
                                    V32{V64{1770709531054080181ull},  V64{7681545774018433657ull}}},
                                V16{V32{V64{14099116470641562327ull}, V64{7494525374756736268ull}},
                                    V32{V64{13581340598962501916ull}, V64{17627949684113770833ull}}}},
                             V8{V16{V32{V64{8670285020770999346ull},  V64{5291415462687816126ull}},
                                    V32{V64{4637184611754716191ull},  V64{12562079378470209571ull}}},
                                V16{V32{V64{16045643123904389068ull}, V64{7905245020499032639ull}},
                                    V32{V64{16444693315434633758ull}, V64{14918613740905007247ull}}}}}},
                       V2{V4{V8{V16{V32{V64{5002792698271100342ull},  V64{4622205550325880879ull}},
                                    V32{V64{10785844482585404038ull}, V64{13146008554385797778ull}}},
                                V16{V32{V64{886379546109508532ull},   V64{2127063706991145372ull}},
                                    V32{V64{15445522489979456714ull}, V64{12947730163231217858ull}}}},
                             V8{V16{V32{V64{3809811818002758893ull},  V64{6326004819218243112ull}},
                                    V32{V64{1953676208133151076ull},  V64{16288970329370822040ull}}},
                                V16{V32{V64{6062887765153225482ull},  V64{312184921085822159ull}},
                                    V32{V64{7018259171661749492ull},  V64{16841955343748996373ull}}}}},
                          V4{V8{V16{V32{V64{9789574564286038075ull},  V64{5341334579611374610ull}},
                                    V32{V64{6242410660354907233ull},  V64{12343346735204933735ull}}},
                                V16{V32{V64{4448874008535595340ull},  V64{10434610894173771290ull}},
                                    V32{V64{7967999038592755366ull},  V64{10784224158795408888ull}}}},
                             V8{V16{V32{V64{10875194899339396674ull}, V64{8690603294288556248ull}},
                                    V32{V64{5921975488022221997ull},  V64{7881097909952940755ull}}},
                                V16{V32{V64{18048002189835756980ull}, V64{5230409584355824932ull}},
                                    V32{V64{17676064915032634942ull}, V64{760957931656866789ull}}}}}}}};
        }
        else
        {
            //           -
            // phi = [1; 1]
        
            constexpr auto M = std::numeric_limits<V>::max();
        
            V p{1};
            V q{1};
        
            V pm2{1};
            V pm1 = p;
            V qm2{0};
            V qm1 = q;
        
            while (true)
            {
                if (pm1 > M - pm2)
                    break;
                p = pm1 + pm2;
                q = qm1 + qm2;
        
                pm2 = pm1;
                pm1 = p;
                qm2 = qm1;
                qm1 = q;
            }
            return R{p, q};
        }
    }();

// Trig functions

template <unsigned N>
constexpr
rational<N>
cos(rational<N> x) noexcept;

template <unsigned N>
constexpr
rational<N>
sin(rational<N> x) noexcept
{
    using namespace bbi;
    using R = rational<N>;
    using V = typename R::value_type;
    auto constexpr two = V{2};
    auto constexpr Pi = pi<R::size>;
    auto constexpr halfPi = Pi/two;
    auto constexpr quarterPi = halfPi/two;
    auto constexpr twoPi = two*Pi;
    bool pneg = false;
    if (x < R{0})
    {
        x = -x;
        pneg = !pneg;
    }
    if (x >= twoPi)
    {
        auto temp = x/twoPi;
        auto k = temp.num() / temp.den();
        x -= k*twoPi;
    }
    if (x >= Pi)
    {
        x -= Pi;
        pneg = !pneg;
    }
    if (x >= halfPi)
    {
        x = Pi - x;
    }
    if (x > quarterPi)
    {
        R y = cos(halfPi - x);
        return pneg ? -R{y} : R{y};
    }
    R y = x;
    bool tneg = true;
    V j{3};
    while (true)
    {
        auto term = power(x, j) / fac(R{j});
        if (term == 0)
            break;
        if (tneg)
            y -= term;
        else
            y += term;
        tneg = !tneg;
        j += two;
    }
    return pneg ? -R{y} : R{y};
}

template <unsigned N>
constexpr
rational<N>
cos(rational<N> x) noexcept
{
    using R = rational<N>;
    using V = typename R::value_type;
    auto constexpr two = V{2};
    auto constexpr Pi = pi<R::size>;
    auto constexpr halfPi = Pi/two;
    auto constexpr quarterPi = halfPi/two;
    auto constexpr twoPi = two*Pi;
    bool pneg = false;
    if (x < R{0})
        x = -x;
    if (x >= twoPi)
    {
        auto temp = x/twoPi;
        auto k = temp.num() / temp.den();
        x -= k*twoPi;
    }
    if (x >= Pi)
    {
        x -= Pi;
        pneg = !pneg;
    }
    if (x >= halfPi)
    {
        x = Pi - x;
        pneg = !pneg;
    }
    if (x > quarterPi)
    {
        R y = sin(halfPi - x);
        return pneg ? -R{y} : R{y};
    }
    R y = V{1};
    V j = two;
    bool tneg = true;
    while (true)
    {
        auto term = power(x, j) / fac(R{j});
        if (term == 0)
            break;
        if (tneg)
            y -= term;
        else
            y += term;
        tneg = !tneg;
        j += two;
    }
    return pneg ? -R{y} : R{y};
}

template <unsigned N>
constexpr
rational<N>
tan(rational<N> x) noexcept
{
    return sin(x)/cos(x);
}

template <unsigned N>
constexpr
rational<N>
cot(rational<N> x) noexcept
{
    return cos(x)/sin(x);
}

template <unsigned N>
constexpr
rational<N>
csc(rational<N> x) noexcept
{
    return sin(x).reciprocal();
}

template <unsigned N>
constexpr
rational<N>
sec(rational<N> x) noexcept
{
    return cos(x).reciprocal();
}

template <unsigned N>
constexpr
rational<N>
asin(rational<N> x) noexcept
{
    using R = rational<N>;
    using V = R::value_type;
    bool pneg = false;
    if (x < R{0})
    {
        x = -x;
        pneg = !pneg;
    }
    if (x == V{1})
        return pneg ? -pi<N>/V{2} : pi<N>/V{2};
    if (x > R{1,2})
    {
        auto y = power((V{1}-x)/V{2}, R{1,2});
        auto z = pi<N>/V{2} - V{2}*asin(y);
        return pneg ? -z : z;
    }
    R z = x;
    R tp = V{1};
    for (auto j = V{1}; true; ++j)
    {
        tp *= R{V{2}*j-V{1}, V{2}*j};
        auto t = tp * power(x, V{2}*j + V{1}) / (V{2}*j + V{1});
        if (t == V{0})
            break;
        z += t;
    }
    return z;
}

template <unsigned N>
constexpr
rational<N>
acos(rational<N> x) noexcept
{
    using R = rational<N>;
    using V = R::value_type;
    return pi<N>/V{2} - asin(x);
}

template <unsigned N>
constexpr
rational<N>
atan(rational<N> x) noexcept
{
    using R = rational<N>;
    using V = R::value_type;
    bool pneg = false;
    if (x < R{0})
    {
        x = -x;
        pneg = !pneg;
    }
    bool invert = false;
    if (x > R{1})
    {
        x.reciprocal();
        invert = true;
    }
    auto constexpr small = R{2} - sqrt3<N>;
    bool reduced = false;
    if (x > small)
    {
        x = R{(x*sqrt3<N> - R{1})/(x + sqrt3<N>)};
        reduced = true;
    }
    auto y = detail::smallarctan(x);
    if (reduced)
        y += pi<N>/V{6};
    if (invert)
        y = pi<N>/V{2} - y;
    if (pneg)
        y = -y;
    return y;
}

template <unsigned N>
constexpr
rational<N>
acot(rational<N> x) noexcept
{
    using R = rational<N>;
    using V = R::value_type;
    return pi<N>/V{2} - atan(x);
}

template <unsigned N>
constexpr
rational<N>
asec(rational<N> x) noexcept
{
    using R = rational<N>;
    if (x == R{0})
        return R{0,0};
    return acos(x.reciprocal());
}

template <unsigned N>
constexpr
rational<N>
acsc(rational<N> x) noexcept
{
    using R = rational<N>;
    if (x == R{0})
        return R{0,0};
    return asin(x.reciprocal());
}

template <unsigned N>
std::string
cf_string(rational<N> r)
{
    std::string s(1, '[');
    if (r.den() != 0)
    {
        auto div = floor_div(r.num(), r.den());
        s += std::string(div.quot);
        s += ';';
        if (div.rem != 0)
        {
            s += ' ';
            r = rational<N>{r.den(), div.rem};
            while (true)
            {
                div = floor_div(r.num(), r.den());
                s += std::string(div.quot);
                if (div.rem == 0)
                    break;
                s += ", ";
                r = rational<N>{r.den(), div.rem};
            }
        }
    }
    else
    {
        s += std::string(r);
    }
    s += ']';
    return s;
}

}  // namespace bbi

#endif  // RATIONAL_H
