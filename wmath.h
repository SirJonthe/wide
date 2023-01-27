#ifndef WMATH_H_INCLUDED__
#define WMATH_H_INCLUDED__

#include <limits>
#include <cmath>
#include "wide.h"

#define wb wide_bool<Depth,Width>
#define sb typename wb::serial_t
#define wi wide_int<Depth,Width>
#define si typename wi::serial_t
#define wu wide_uint<Depth,Width>
#define su typename wu::serial_t
#define wf wide_float<Depth,Width>
#define sf typename wf::serial_t
#define sw typename wide_t::serial_t
#define nan wf(std::numeric_limits<sf>::quiet_NaN())
#define eps std::numeric_limits<sf>::epsilon()

namespace wide
{


/**
 * Returns the highest value.
 * 
 * \param a a value.
 * \param b a value.
 * 
 * \returns the higher value.
 * 
 * \sa min
 */
template < typename wide_t >
inline wide_t max(const wide_t &a, const wide_t &b)
{
	return wide::cmov(a < b, b, a);
}


/**
 * Returns the smallest value.
 * 
 * \param a a value.
 * \param b a value.
 * 
 * \returns the smaller value.
 * 
 * \sa max
 */
template < typename wide_t >
inline wide_t min(const wide_t &a, const wide_t &b)
{
	return wide::cmov(a < b, a, b);
}


/**
 * Returns the positive value.
 * 
 * \param x a value.
 * 
 * \returns the positive value.
 */
template < typename wide_t >
inline wide_t abs(const wide_t &x)
{
	return wide::max(x, -x);
}


/**
 * Clamps a value inside a given inclusive range. Values below the lower bound returns the lower bound, while values above the higher bound returns the higher bound. All other values are returned as-is.
 * 
 * \param lo the lower bound.
 * \param x a value to be clamped.
 * \param hi the higher bound.
 * 
 * \returns the clamped value.
 * 
 * \sa wrap
 */
template < typename wide_t >
inline wide_t clamp(const wide_t &lo, const wide_t &x, const wide_t &hi)
{
	return min(max(x, lo), hi);
}


/**
 * Returns the fractions of a given floating-point value.
 * 
 * \param x a floating-point value.
 * 
 * \returns the fraction.
 */
template < uint32_t Depth, uint32_t Width >
inline wf frac(const wf &x)
{
	return x - wf(wi(x));
}


/**
 * Returns the integer and fractional part of a floating-point value.
 * 
 * \param x a floating-value.
 * \param intpart the output integer part of the input floating-point value 'x'.
 * 
 * \returns the fraction.
 */
template < uint32_t Depth, uint32_t Width >
inline wf modf(const wf &x, const wf *intpart)
{
	*intpart = wi(x);
	return x - *intpart;
}


/**
 * Returns the integer of an input floating-point value by rounding down positive numbers and rounding up negative numbers.
 * 
 * \param x a floating-point value.
 * 
 * \note This function properly handles negative numbers, unlike casting which truncates the fractional part.
 * 
 * \returns the floored value.
 * 
 * \sa ceil
 * \sa round
 */
template < uint32_t Depth, uint32_t Width >
inline wf floor(const wf &x)
{
	const wf F = frac(x);
	return wide::cmov(F != wf(sf(0)), wide::cmov(x >= 0, x - F, x - (wf(sf(1)) + F)), x);
}


/**
 * Returns the integer of an input floating-point value by rounding up positive numbers and rounding down negative numbers.
 * 
 * \param x a floating-point value.
 * 
 * \returns the ceiled value.
 * 
 * \sa floor
 * \sa round
 */
template < uint32_t Depth, uint32_t Width >
inline wf ceil(const wf &x)
{
	const wf F = frac(x);
	return wide::cmov(F != wf(sf(0)), wide::cmov(x >= sf(0), x + (wf(sf(1)) - F), x - F), x);
}


/**
 * Returns the integer of an input floating-point value by conventional rounding.
 * 
 * \param x floating-point value.
 * 
 * \returns the rounded value.
 * 
 * \sa floor
 * \sa ceil
 */
template < uint32_t Depth, uint32_t Width >
inline wf round(const wf &x)
{
	const wf offset = wide::cmov(x >= sf(0), wf(sf(0.5)), wf(sf(-0.5)));
	return wf(wi(x + offset));
}


/**
 * Returns the sign of the input value.
 * 
 * \param x a value.
 * 
 * \returns the sign of the value as an integer; -1 for negative, 0 for 0, and 1 for positive.
 */
template < typename wide_t >
inline wide_t sign(const wide_t &x)
{
	return wide::cmov(x >= wide_t(sw(0)), wide_t(sw(1)), wide_t(sw(-1)));
}


/**
 * Returns the wrapped input floating-point value by repeating it within the real interval [0, 1).
 * 
 * \param x a floating-point value.
 * 
 * \returns the wrapped value.
 */
template < uint32_t Depth, uint32_t Width >
inline wf wrap(const wf &x)
{
	return x - wide::floor(x);
}


/**
 * Returns the wrapped input floating-point value by repeating it within the real interval [0, max).
 * 
 * \param x a floating-point value.
 * \param max the upper bound of the range. Non-inclusive.
 * 
 * \returns the wrapped value.
 */
template < uint32_t Depth, uint32_t Width >
inline wf wrap(const wf &x, const wf &max)
{
	wf ratio = x / max;
	return max * wide::wrap(ratio);
}

/**
 * Returns the wrapped input floating-point value by repeating it within the real interval [min, max)
 * 
 * \param min the lower bound of the range. Inclusive.
 * \param x a floating-point value.
 * \param max the upper bound of the range. Non-inclusive.
 * 
 * \returns the wrapped value.
 * 
 * \sa clamp
 */
template < uint32_t Depth, uint32_t Width >
inline wf wrap(const wf &min, const wf & x, const wf &max)
{
	return wide::wrap(x - min, max - min) + min;
}

/*template < uint32_t Depth, uint32_t Width >
wf pi( void )
{
	// Nilakantha series
	wf pi = wf(3);
	const int iter = Depth * 4;
	for (int i = 0; i < iter; i += 4) {
		pi = pi + (wf(4) / wf((i+2) * (i+3) * (i+4))) - (wf(4) / wf((i+4) * (i+5) * (i+6)));
	}
	return pi;
}*/

#define WIDE_E        2.7182818 // Euler's number or exp(1)
#define WIDE_LN2      0.6931472 // Natural logarithm of 2 or log(2)
#define WIDE_LN10     2.3025851 // Natural logarithm of 10 or log(10)

#define WIDE_LOG10E   0.4342945 // Log of E (Euler's number) at base 10
#define WIDE_LOG2E    1.4426950 // Log of E (Euler's number) at base 2

#define WIDE_SQRT2    1.4142136 // Square root of 2 or sqrt(2)
#define WIDE_SQRT1_2  0.7071068 // Square root of 1/2 or sqrt(1/2) or 1/sqrt(2)

#define WIDE_PI       3.1415927 // PI number
#define WIDE_PI_2     1.5707963 // PI/2
#define WIDE_PI_4     0.7853982 // PI/4
#define WIDE_1_PI     0.3183099 // 1/PI
#define WIDE_2_PI     0.6366197 // 2/PI
#define WIDE_2_SQRTPI 1.1283792 // 2/sqrt(PI)


/**
 * Returns an approximation of sine of the input floating-point radians.
 * 
 * \param rad input floating-point radians.
 * 
 * \returns the sine.
 * 
 * \sa cos
 * \sa tan
 * \sa asin
 */
template < uint32_t Depth, uint32_t Width >
wf sin(wf rad)
{
	const wf PI = sf(WIDE_PI);
	rad = wide::wrap(-PI, rad, PI);
	const wf B = wf(sf(4)) / PI; // Magic value 1
	const wf C = B / PI;         // Magic value 2
	const wf sin1 = B * rad - C * rad * wide::abs(rad);
	const wf sin  = wf(sf(0.225)) * (sin1 * wide::abs(sin1) - sin1) + sin1;
	return sin;
}

#define F0(x) (A * ((B * x - C * x * x) * wide::abs(B * x - C * x * x) - (B * x - C * x * x)) + (B * x - C * x * x) - S)
#define F1(x) ((B - D * C * x) * (A * wide::abs(B - D * C * x) - A - D * C * x))

template < uint32_t Depth, uint32_t Width >
wf asin_nr(wf S)
{
	const wf PI = sf(WIDE_PI);
	const wf A = wf(sf(0.225));
	const wf B = wf(sf(4)) / PI;
	const wf C = B / PI;
	const wf D = wf(sf(2));
	const wf EPS = wf(sf(0.01));

	wb mask = true;

	wf x0 = wf(sf(-1));
	wf x1 = x0 - F0(x0) / F1(x0);

	WIDE_WHILE(wide::abs(x1 - x0) < EPS)
		x0 = WIDE_SET(x1);
		x1 = WIDE_SET(x0 - F0(x0) / F1(x0));
	END_WIDE_WHILE

	return x0;
}

#undef F0
#undef F1

#define F(x) (A * ((B * x - C * x * x) * abs(B * x - C * x * x) - (B * x - C * x * x)) + (B * x - C * x * x) - S)

template < uint32_t Depth, uint32_t Width >
wf asin_bs(wf S)
{
	const wf PI = sf(WIDE_PI);
	const wf A = wf(sf(0.225));
	const wf B = wf(sf(4)) / PI;
	const wf C = B / PI;
	const wf D = wf(sf(2));
	const wf EPS = wf(sf(0.01));

	wb mask = true;

	wf a = -PI;
	wf b = PI;
	WIDE_WHILE(wide::abs(b - a) > EPS)
		wf c = (a + b) / 2;
		WIDE_IF(F(c) * F(a) < 0)
			b = WIDE_SET(c);
		WIDE_ELSE
			a = WIDE_SET(c);
		END_WIDE_IF
	END_WIDE_WHILE
	return (a + b) / 2;
}

#undef F


/**
 * Returns the cosine of the input floating-point radians.
 * 
 * \param rad input floating-point radians.
 * 
 * \returns the cosine.
 * 
 * \sa sin
 * \sa tan
 * \sa acos
 */
template < uint32_t Depth, uint32_t Width >
wf cos(const wf &rad)
{
	return wide::sin(wf(WIDE_PI_2) - rad);
}


/**
 * Returns the tangent of the input floating-point radians.
 * 
 * \param rad input floating-point radians.
 * 
 * \returns the tangent.
 * 
 * \sa sin
 * \sa cos
 * \sa atan
 */
template < uint32_t Depth, uint32_t Width >
wf tan(const wf &rad)
{
	return wide::sin(rad) / wide::cos(rad);
}

/*template < uint32_t Depth, uint32_t Width >
wf sqrt_nr(const wf &x)
{
	const wb not_nan = (x >= 0);
	const wf diff = eps;
	const wf half = 0.5;
	wf guess = 1;
	wf last_guess;
	wb mask = not_nan;

	WIDE_DOWHILE
		last_guess = guess;
		guess = WIDE_SET((x / guess + guess) * half);
	END_WIDE_DOWHILE(last_guess != guess && wide::abs(guess * guess - x) >= diff);

	WIDE_IF(!not_nan)
		guess = WIDE_SET(nan);
	END_WIDE_IF

	return guess;
}

template < uint32_t Depth, uint32_t Width >
wf sqrt_bs(const wf &x)
{
	const wf half = 0.5;
	const wf p100 = 100;
	const wf p001 = 0.01;
	const wf p10  = 10;
	const wf p01  = 0.1;

	const wb not_nan = (x >= 0.0);
	wb mask = not_nan;

	wf lo = wide::min(wf(1), x);
	wf hi = wide::max(wf(1), x);
	wf mid;

	WIDE_WHILE(p100 * lo * lo < x)
		lo = WIDE_SET(lo * p10);
	END_WIDE_WHILE

	WIDE_WHILE(p001 * hi * hi > x)
		hi = WIDE_SET(hi * p01);
	END_WIDE_WHILE

	wi i = 0;
	WIDE_WHILE(i < Depth)
		mid = WIDE_SET((lo + hi) * half);
		const wf mid2 = mid * mid;
		WIDE_IF(mid2 != x)
			WIDE_IF(mid2 > x)
				hi = WIDE_SET(mid);
			WIDE_ELSE
				lo = WIDE_SET(mid);
			END_WIDE_IF
		END_WIDE_IF
		++i;
	END_WIDE_WHILE
	
	WIDE_IF(!not_nan)
		mid = WIDE_SET(nan);
	END_WIDE_IF
	
	return mid;
}*/


/**
 * Returns the square root of the input floating-point number via Newton-Raphson method.
 * 
 * \param x input floating-point value.
 * 
 * \returns the square root.
 * 
 * \sa sqrt_bs
 */
template < uint32_t Depth, uint32_t Width >
wf sqrt_nr(const wf &x)
{
	const wb not_nan = (x >= sf(0.0));
	wf guess = 1;
	if (not_nan) {
		const wf diff = eps;
		const wf half = sf(0.5);
		wf last_guess;
		wb m = not_nan;
		do {
			last_guess = guess;
			guess = wide::cmov(m, (x / guess + guess) * half, guess);
		} while (m &= (last_guess != guess && wide::abs(guess * guess - x) >= diff));
	}
	guess = wide::cmov(not_nan, guess, nan);
	return guess;
}


/**
 * Returns the square root of the input floating-point number via a binary partitioning method.
 * 
 * \param x input floating-point value.
 * 
 * \returns the square root.
 * 
 * \sa sqrt_nr
 */
template < uint32_t Depth, uint32_t Width >
wf sqrt_bs(const wf &x)
{
	const wf half = sf(0.5);
	const wf p100 = sf(100);
	const wf p001 = sf(0.01);
	const wf p10  = sf(10);
	const wf p01  = sf(0.1);

	const wb not_nan = (x >= sf(0.0));
	wb m0;
	wb m1;

	wf lo = wide::min(wf(sf(1)), x);
	wf hi = wide::max(wf(sf(1)), x);
	wf mid;

	m0 = not_nan;
	while ((m0 &= (p100 * lo * lo < x))) {
		lo = wide::cmov(m0, lo * p10, lo);
	}

	m0 = not_nan;
	while ((m0 &= (p001 * hi * hi > x))) {
		hi = wide::cmov(m0, hi * p01, hi);
	}

	m0 = not_nan;
	for (int i = 0 ; i < Depth; ++i){
		mid = wide::cmov(m0, (lo + hi) * half, mid);

		m0 &= !(mid * mid == x);
		if (m0) {
			m1 = (mid * mid > x);
			if (m1)  { hi = wide::cmov(m1, mid, hi); }
			m1 = !m1;
			if (m1) { lo = wide::cmov(m1, mid, lo); }
		} else {
			break;
		}
	}
	mid = wide::cmov(not_nan, mid, nan);
	return mid;
}


/**
 * Returns a boolean indicating if the input integer is even or not.
 * 
 * \param x input integer value.
 * 
 * \returns true for even, false if not.
 * 
 * \sa odd
 */
template < uint32_t Depth, uint32_t Width >
wb even(const wi &x)
{
	return (x % wi(si(2))) == wi(si(0));
}

/**
 * Returns a boolean indicating if the input integer is odd or not.
 * 
 * \param x input integer value.
 * 
 * \returns true for odd, false if not.
 * 
 * \sa even
 */
template < uint32_t Depth, uint32_t Width >
wb odd(const wi &x)
{
	return (x % wi(si(2))) == wi(si(1));
}

/*template < uint32_t Depth, uint32_t Width >
wf pow(wf x, wi n)
{
	wb m0 = (n != 0);
	wb m1;
	if (m0) {
		x = wide::cmov(m0, x, wf(1));
	} else {
		return wf(1);
	}

	m0 &= (n >= 0);
	x = wide::cmov(m0, x, wf(1) / x);
	n = wide::cmov(m0, n, -n);

	wf y = 1;

	while ((m0 &= n > 1)) {
		m1 = even(n) & m0;
		if (m1) {
			x = wide::cmov(m1, x * x, x);
			n = wide::cmov(m1, n / wi(2), n);
		}
		m1 = (!m1) & m0;
		if (m1) {
			y = wide::cmov(m1, x * y, y);
			x = wide::cmov(m1, x * x, x);
			n = wide::cmov(m1, (n - wi(1)) / wi(2), n);
		}
	}

	return x * y;
}*/


/**
 * Returns the base-2 logarithm of the input integer.
 * 
 * \param n input integer value.
 * 
 * \returns the base-2 logarithm of the integer.
 * 
 * \sa log10
 */
template < uint32_t Depth, uint32_t Width >
wi log2(wi n)
{
	wb m0 = true;
	wi logValue = si(-1);
	while ((m0 &= (n != 0))) {
		logValue = wide::cmov(m0, logValue + wi(1), logValue);
		n = wide::cmov(m0, n >> 1, n);
	}
	return logValue;
}


/**
 * Returns the base-10 logarithm of the input integer.
 * 
 * \param b input integer value.
 * 
 * \returns the base-10 logarithm of the integer.
 * 
 * \sa log2
 */
template < uint32_t Depth, uint32_t Width >
wi log10(const wi &n)
{
	return wide::log2(n) / wide::log2<Depth,Width>(10);
}


/**
 * Raises the input by an exponent.
 * 
 * \param base the base floating-point value to be raised by an exponent.
 * \param ex the floating-point exponent.
 * 
 * \returns the result of raising the base by the exponent.
 */
template < uint32_t Depth, uint32_t Width >
wf pow(const wf &base, const wf &ex);


/**
 * Returns a number which, when raised by 'n' yields 'A'.
 * 
 * \param A input floating-point base.
 * \param n the input integer exponent.
 * 
 * \returns true for even, false if not.
 */
template < uint32_t Depth, uint32_t Width >
wf nth_root(const wf &A, const wi &n)
{
	constexpr int K = 6;
	wf x[K] = { wf(1) };
	const wf N = wf(n);
	for (int k = 0; k < K - 1; ++k) {
		x[k + 1] = (wf(sf(1)) / N) * ((N - wf(sf(1))) * x[k] + A / wide::pow(x[k], N - wf(sf(1))));
	}
	return x[K - 1];
}


/**
 * Raises the input by an exponent.
 * 
 * \param base the base floating-point value to be raised by an exponent.
 * \param ex the floating-point exponent.
 * 
 * \returns the result of raising the base by the exponent.
 */
template < uint32_t Depth, uint32_t Width >
wf pow(const wf &base, const wf &ex)
{
	wf out = 0.0;

	// power of 0
	wb m0 = (base != 0);
	wb m1 = (ex == 0 && m0);
	if (m1) {
		out = wide::cmov(m1, wf(1), out);
	}
	m1 = !m1 && m0;
	if (m1) {
		// negative exponenet
		wb m2 = (ex < 0 && m1);
		if (m2) {
			out = wide::cmov(m2, wf(sf(1)) / wide::pow(base, -ex), out);
		}
		m2 = !m2 && m1;
		if (m2) {
			// fractional exponent
			wb m3 = (ex > 0 && ex < 1 && m2);
			if (m3) {
				out = wide::cmov(m3, wide::nth_root(base, wi(wf(sf(1)) / ex)), out);
			}
			m3 = !m3 && m2;
			if (m3) {
				wb m4 = (wi(ex) % wi(si(2)) == 0 && m3);
				if (m4) {
					const wf half_pow = wide::pow(base, ex / wf(sf(2)));
					out = wide::cmov(m4, half_pow * half_pow, out);
				}
				m4 = !m4 && m3;
				if (m4) {
					// integer exponenet
					out = wide::cmov(m4, base * wide::pow(base, ex - wf(1)), out);
				}
			}
		}
	}
	return out;
}

/*template < uint32_t Depth, uint32_t Width >
wf pow(const wf &x, const wf &n)
{
	return wide::exp(log(x) * b);
}*/

}

#undef wb
#undef sb
#undef wi
#undef si
#undef wu
#undef su
#undef wf
#undef sf
#undef sw
#undef nan
#undef eps

#endif // WMATH_H_INCLUDED__
