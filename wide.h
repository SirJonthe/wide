#ifndef WIDE_H_INCLUDED__
#define WIDE_H_INCLUDED__

#include <cstdint>
#include <limits>
#include <cstdlib>

#define FOR(x) for (uint32_t i = 0; i < Width; ++i) { x; }
#define CMP(sign) wide_bool<Depth,Width> o; for (uint32_t i = 0; i < Width; ++i) { o.v[i] = v[i] sign r.v[i] ? wide_bool<Depth,Width>::TRUE_BITS : wide_bool<Depth,Width>::FALSE_BITS; } return o
#define CMP1(sign) wide_bool<Depth,Width> o; for (uint32_t i = 0; i < Width; ++i) { o.v[i] = v[i] sign r ? wide_bool<Depth,Width>::TRUE_BITS : wide_bool<Depth,Width>::FALSE_BITS; } return o

#define ASSOP(type, op) \
	type &operator op(const type &r)                    { FOR(v[i] op r.v[i]) return *this; } \
	type &operator op(const typename type::serial_t &r) { FOR(v[i] op r) return *this; }

#define UNIOP(type, op) \
	type operator op( void ) const { type o; FOR(o.v[i] = op v[i]) return o; }

#define INCOP(type, op) \
	type &operator op( void ) { FOR(op v[i]) return *this; } \
	type  operator op( int )  { type o = *this; FOR(o.v[i] = op v[i]); return o; }

#define CMPOP(type, op) \
	wide_bool<Width,Depth> operator op(const type &r) const                    { CMP(op); } \
	wide_bool<Width,Depth> operator op(const typename type::serial_t &r) const { CMP1(op); }

#define WIDE_IF(condition) \
	{ \
		const auto &mask0 = mask; \
		auto mask = (condition) & mask0; \
		if (bool(mask)) {
#define WIDE_ELSE \
		} \
		mask = !mask & mask0; \
		if (bool(mask)) {
#define END_WIDE_IF \
		} \
	}

#define WIDE_WHILE(condition) \
	{ \
		const auto &mask0 = mask; \
		do { \
			auto mask = (condition) & mask0; \
			if (bool(mask)) {
#define END_WIDE_WHILE \
			} else { \
				break; \
			} \
		} while (true); \
	}


#define WIDE_DOWHILE \
	{ \
		const auto &mask0 = mask; \
		auto mask = mask0; \
		do { \
			if (bool(mask)) {
#define END_WIDE_DOWHILE(condition) \
			} else { \
				break; \
			} \
			mask = (condition) & mask0; \
		} while (true); \
	}

#define WIDE_SET(l) wide::cset<decltype(l)>{ mask, l }

namespace wide
{

template < uint32_t bits >
class __wide_types {};

template <>
class __wide_types<8>
{
public:
	typedef uint8_t uint_t;
	typedef int8_t  int_t;
	typedef uint8_t bool_t;
};

template <>
class __wide_types<16>
{
public:
	typedef uint16_t uint_t;
	typedef int16_t  int_t;
	typedef uint16_t bool_t;
};

template <>
class __wide_types<32>
{
public:
	typedef uint32_t uint_t;
	typedef int32_t  int_t;
	typedef uint32_t bool_t;
	typedef float    float_t;
};

template <>
class __wide_types<64>
{
public:
	typedef uint64_t uint_t;
	typedef int64_t  int_t;
	typedef uint64_t bool_t;
	typedef double   float_t;
};

template < uint32_t Depth, uint32_t Width > class wide_bool;
template < uint32_t Depth, uint32_t Width > class wide_int;
template < uint32_t Depth, uint32_t Width > class wide_uint;
template < uint32_t Depth, uint32_t Width > class wide_float;

template < typename wide_t >
struct cset
{
	const wide_bool<wide_t::depth,wide_t::width> &mask;
	const wide_t                                  a;
};

template < uint32_t Depth, uint32_t Width >
class alignas(Width * sizeof(typename __wide_types<Depth>::uint_t)) wide_bool
{
public:
	typedef typename __wide_types<Depth>::uint_t serial_t;
	static constexpr uint32_t width = Width;
	static constexpr uint32_t depth = Depth;
	friend class wide_int<Depth,Width>;
	friend class wide_uint<Depth,Width>;
	friend class wide_float<Depth,Width>;

private:
	serial_t v[Width];

private:
	static constexpr serial_t FALSE_BITS = 0;
	static constexpr serial_t TRUE_BITS  = std::numeric_limits<serial_t>::max();

public:
	wide_bool( void ) = default;
	wide_bool(const wide_bool&) = default;
	wide_bool(bool r) { FOR(v[i] = r ? TRUE_BITS : FALSE_BITS) }
	explicit wide_bool(const bool *r) { FOR(v[i] = r[i] ? TRUE_BITS : FALSE_BITS) }
	explicit wide_bool(const wide_int<Depth,Width> &r) { FOR(v[i] = r.v[i] ? TRUE_BITS : FALSE_BITS); }
	wide_bool &operator=(const wide_bool&) = default;
	wide_bool &operator=(bool r) { FOR(v[i] = r ? TRUE_BITS : FALSE_BITS) return *this; }
	wide_bool &operator=(const cset<wide_bool> &test) { *this = cmov(test.mask, test.a, *this); return *this; }
	wide_bool &operator=(const cset<const wide_bool> &test) { *this = cmov(test.mask, test.a, *this); return *this; }

	wide_bool operator==(const wide_bool &r) const { CMP(==); }
	wide_bool operator!=(const wide_bool &r) const { CMP(!=); }
	wide_bool operator< (const wide_bool &r) const { CMP(<); }
	wide_bool operator> (const wide_bool &r) const { CMP(>); }
	wide_bool operator&&(const wide_bool &r) const { return (*this) & r; }
	wide_bool operator||(const wide_bool &r) const { return (*this) | r; }

	wide_bool &operator&=(const wide_bool &r) { FOR(v[i] &= r.v[i]) return *this; }
	wide_bool &operator|=(const wide_bool &r) { FOR(v[i] |= r.v[i]) return *this; }
	wide_bool &operator^=(const wide_bool &r) { FOR(v[i] ^= r.v[i]) return *this; }
	wide_bool operator!( void ) const { wide_bool o; FOR(o.v[i] = ~v[i]) return o; }

	operator bool( void ) const { serial_t o = 0; FOR(o |= v[i]) return o ? true : false; }

	static wide_bool wide_true( void )  { return wide_bool(true); }
	static wide_bool wide_false( void ) { return wide_bool(false); }
};

template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> TRUE( void ) { return wide_bool<Depth,Width>(true); }
template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> FALSE( void ) { return wide_bool<Depth,Width>(false); }

template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> operator&(wide_bool<Depth,Width> l, const wide_bool<Depth,Width> &r) { return l &= r; }
template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> operator|(wide_bool<Depth,Width> l, const wide_bool<Depth,Width> &r) { return l |= r; }
template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> operator^(wide_bool<Depth,Width> l, const wide_bool<Depth,Width> &r) { return l ^= r; }

template < uint32_t Depth, uint32_t Width >
class alignas(Width * sizeof(typename __wide_types<Depth>::int_t)) wide_int
{
public:
	typedef typename __wide_types<Depth>::int_t serial_t;
	static constexpr uint32_t width = Width;
	static constexpr uint32_t depth = Depth;
	friend class wide_float<Depth,Width>;

private:
	serial_t v[Width];

public:
	wide_int( void ) = default;
	wide_int(const wide_int&) = default;
	wide_int(serial_t r) { FOR(v[i] = r) }
	explicit wide_int(const serial_t *r) { FOR(v[i] = r[i]) }
	explicit wide_int(const wide_float<Depth,Width> &r) { FOR(v[i] = r.v[i]) }
	explicit wide_int(const wide_bool<Depth,Width> &r)  { FOR(v[i] = r.v[i] ? 1 : 0) }
	wide_int &operator=(const wide_int&) = default;
	wide_int &operator=(serial_t r) { FOR(v[i] = r) return *this; }
	wide_int &operator=(const cset<wide_int> &test) { *this = cmov(test.mask, test.a, *this); return *this; }
	wide_int &operator=(const cset<const wide_int> &test) { *this = cmov(test.mask, test.a, *this); return *this; }

	wide_int &operator+= (const wide_int &r) { FOR(v[i] +=  r.v[i]) return *this; }
	wide_int &operator-= (const wide_int &r) { FOR(v[i] -=  r.v[i]) return *this; }
	wide_int &operator*= (const wide_int &r) { FOR(v[i] *=  r.v[i]) return *this; }
	wide_int &operator/= (const wide_int &r) { FOR(v[i] /=  r.v[i]) return *this; }
	wide_int &operator%= (const wide_int &r) { FOR(v[i] %=  r.v[i]) return *this; }
	wide_int &operator<<=(const wide_int &r) { FOR(v[i] <<= r.v[i]) return *this; }
	wide_int &operator>>=(const wide_int &r) { FOR(v[i] >>= r.v[i]) return *this; }
	wide_int &operator+= (serial_t r)        { FOR(v[i] +=  r)      return *this; }
	wide_int &operator-= (serial_t r)        { FOR(v[i] -=  r)      return *this; }
	wide_int &operator*= (serial_t r)        { FOR(v[i] *=  r)      return *this; }
	wide_int &operator/= (serial_t r)        { FOR(v[i] /=  r)      return *this; }
	wide_int &operator%= (serial_t r)        { FOR(v[i] %=  r)      return *this; }
	wide_int &operator<<=(serial_t r)        { FOR(v[i] <<= r)      return *this; }
	wide_int &operator>>=(serial_t r)        { FOR(v[i] >>= r)      return *this; }

	wide_int  operator-( void ) const        { wide_int o; FOR(o.v[i] = -v[i]) return o; }
	
	wide_int &operator++( void ) { FOR(++v[i]) return *this; }
	wide_int  operator++( int )  { wide_int o = *this; FOR(++v[i]); return o; }
	wide_int &operator--( void ) { FOR(--v[i]) return *this; }
	wide_int  operator--( int )  { wide_int o = *this; FOR(--v[i]); return o; }

	wide_bool<Depth,Width> operator==(const wide_int &r) const { CMP(==); }
	wide_bool<Depth,Width> operator!=(const wide_int &r) const { CMP(!=); }
	wide_bool<Depth,Width> operator <(const wide_int &r) const { CMP(<); }
	wide_bool<Depth,Width> operator >(const wide_int &r) const { CMP(>); }
	wide_bool<Depth,Width> operator<=(const wide_int &r) const { CMP(<=); }
	wide_bool<Depth,Width> operator>=(const wide_int &r) const { CMP(>=); }
};

template < uint32_t Depth, uint32_t Width > wide_int<Depth,Width> operator+ (wide_int<Depth,Width> l, const wide_int<Depth,Width> &r) { return l +=  r; }
template < uint32_t Depth, uint32_t Width > wide_int<Depth,Width> operator- (wide_int<Depth,Width> l, const wide_int<Depth,Width> &r) { return l -=  r; }
template < uint32_t Depth, uint32_t Width > wide_int<Depth,Width> operator* (wide_int<Depth,Width> l, const wide_int<Depth,Width> &r) { return l *=  r; }
template < uint32_t Depth, uint32_t Width > wide_int<Depth,Width> operator/ (wide_int<Depth,Width> l, const wide_int<Depth,Width> &r) { return l /=  r; }
template < uint32_t Depth, uint32_t Width > wide_int<Depth,Width> operator% (wide_int<Depth,Width> l, const wide_int<Depth,Width> &r) { return l %=  r; }
template < uint32_t Depth, uint32_t Width > wide_int<Depth,Width> operator<<(wide_int<Depth,Width> l, const wide_int<Depth,Width> &r) { return l <<= r; }
template < uint32_t Depth, uint32_t Width > wide_int<Depth,Width> operator>>(wide_int<Depth,Width> l, const wide_int<Depth,Width> &r) { return l >>= r; }

template < uint32_t Depth, uint32_t Width > wide_int<Depth,Width> operator+ (wide_int<Depth,Width> l, typename wide_int<Depth,Width>::serial_t r) { return l +=  r; }
template < uint32_t Depth, uint32_t Width > wide_int<Depth,Width> operator- (wide_int<Depth,Width> l, typename wide_int<Depth,Width>::serial_t r) { return l -=  r; }
template < uint32_t Depth, uint32_t Width > wide_int<Depth,Width> operator* (wide_int<Depth,Width> l, typename wide_int<Depth,Width>::serial_t r) { return l *=  r; }
template < uint32_t Depth, uint32_t Width > wide_int<Depth,Width> operator/ (wide_int<Depth,Width> l, typename wide_int<Depth,Width>::serial_t r) { return l /=  r; }
template < uint32_t Depth, uint32_t Width > wide_int<Depth,Width> operator% (wide_int<Depth,Width> l, typename wide_int<Depth,Width>::serial_t r) { return l %=  r; }
template < uint32_t Depth, uint32_t Width > wide_int<Depth,Width> operator<<(wide_int<Depth,Width> l, typename wide_int<Depth,Width>::serial_t r) { return l <<= r; }
template < uint32_t Depth, uint32_t Width > wide_int<Depth,Width> operator>>(wide_int<Depth,Width> l, typename wide_int<Depth,Width>::serial_t r) { return l >>= r; }

template < uint32_t Depth, uint32_t Width > wide_int<Depth,Width> operator+ (typename wide_int<Depth,Width>::serial_t l, const wide_int<Depth,Width> &r) { return wide_int<Depth,Width>(l) +=  r; }
template < uint32_t Depth, uint32_t Width > wide_int<Depth,Width> operator- (typename wide_int<Depth,Width>::serial_t l, const wide_int<Depth,Width> &r) { return wide_int<Depth,Width>(l) -=  r; }
template < uint32_t Depth, uint32_t Width > wide_int<Depth,Width> operator* (typename wide_int<Depth,Width>::serial_t l, const wide_int<Depth,Width> &r) { return wide_int<Depth,Width>(l) *=  r; }
template < uint32_t Depth, uint32_t Width > wide_int<Depth,Width> operator/ (typename wide_int<Depth,Width>::serial_t l, const wide_int<Depth,Width> &r) { return wide_int<Depth,Width>(l) /=  r; }
template < uint32_t Depth, uint32_t Width > wide_int<Depth,Width> operator% (typename wide_int<Depth,Width>::serial_t l, const wide_int<Depth,Width> &r) { return wide_int<Depth,Width>(l) %=  r; }
template < uint32_t Depth, uint32_t Width > wide_int<Depth,Width> operator<<(typename wide_int<Depth,Width>::serial_t l, const wide_int<Depth,Width> &r) { return wide_int<Depth,Width>(l) <<= r; }
template < uint32_t Depth, uint32_t Width > wide_int<Depth,Width> operator>>(typename wide_int<Depth,Width>::serial_t l, const wide_int<Depth,Width> &r) { return wide_int<Depth,Width>(l) >>= r; }

template < uint32_t Depth, uint32_t Width >
class alignas(Width * sizeof(typename __wide_types<Depth>::uint_t)) wide_uint
{
public:
	typedef typename __wide_types<Depth>::uint_t serial_t;
	static constexpr uint32_t width = Width;
	static constexpr uint32_t depth = Depth;
	friend class wide_float<Depth,Width>;

private:
	serial_t v[Width];

public:
	wide_uint( void ) = default;
	wide_uint(const wide_uint&) = default;
	wide_uint(serial_t r) { FOR(v[i] = r) }
	explicit wide_uint(const serial_t *r) { FOR(v[i] = r[i]) }
	explicit wide_uint(const wide_float<Depth,Width> &r) { FOR(v[i] = r.v[i]) }
	explicit wide_uint(const wide_bool<Depth,Width> &r)  { FOR(v[i] = r.v[i] ? 1 : 0) }
	wide_uint &operator=(const wide_uint&) = default;
	wide_uint &operator=(serial_t r) { FOR(v[i] = r) return *this; }
	wide_uint &operator=(const cset<wide_uint> &test) { *this = cmov(test.mask, test.a, *this); return *this; }
	wide_uint &operator=(const cset<const wide_uint> &test) { *this = cmov(test.mask, test.a, *this); return *this; }

	wide_uint &operator+= (const wide_uint &r) { FOR(v[i] +=  r.v[i]) return *this; }
	wide_uint &operator-= (const wide_uint &r) { FOR(v[i] -=  r.v[i]) return *this; }
	wide_uint &operator*= (const wide_uint &r) { FOR(v[i] *=  r.v[i]) return *this; }
	wide_uint &operator/= (const wide_uint &r) { FOR(v[i] /=  r.v[i]) return *this; }
	wide_uint &operator%= (const wide_uint &r) { FOR(v[i] %=  r.v[i]) return *this; }
	wide_uint &operator<<=(const wide_uint &r) { FOR(v[i] <<= r.v[i]) return *this; }
	wide_uint &operator>>=(const wide_uint &r) { FOR(v[i] >>= r.v[i]) return *this; }
	wide_uint &operator+= (serial_t r)         { FOR(v[i] +=  r)      return *this; }
	wide_uint &operator-= (serial_t r)         { FOR(v[i] -=  r)      return *this; }
	wide_uint &operator*= (serial_t r)         { FOR(v[i] *=  r)      return *this; }
	wide_uint &operator/= (serial_t r)         { FOR(v[i] /=  r)      return *this; }
	wide_uint &operator%= (serial_t r)         { FOR(v[i] %=  r)      return *this; }
	wide_uint &operator<<=(serial_t r)         { FOR(v[i] <<= r)      return *this; }
	wide_uint &operator>>=(serial_t r)         { FOR(v[i] >>= r)      return *this; }

	wide_uint  operator-( void ) const         { wide_uint o; FOR(o.v[i] = -v[i]) return o; }
	
	wide_uint &operator++( void ) { FOR(++v[i]) return *this; }
	wide_uint  operator++( int )  { wide_uint o = *this; FOR(++v[i]); return o; }
	wide_uint &operator--( void ) { FOR(--v[i]) return *this; }
	wide_uint  operator--( int )  { wide_uint o = *this; FOR(--v[i]); return o; }

	wide_bool<Depth,Width> operator==(const wide_uint &r) const { CMP(==); }
	wide_bool<Depth,Width> operator!=(const wide_uint &r) const { CMP(!=); }
	wide_bool<Depth,Width> operator <(const wide_uint &r) const { CMP(<); }
	wide_bool<Depth,Width> operator >(const wide_uint &r) const { CMP(>); }
	wide_bool<Depth,Width> operator<=(const wide_uint &r) const { CMP(<=); }
	wide_bool<Depth,Width> operator>=(const wide_uint &r) const { CMP(>=); }
};

template < uint32_t Depth, uint32_t Width > wide_uint<Depth,Width> operator+ (wide_uint<Depth,Width> l, const wide_uint<Depth,Width> &r) { return l +=  r; }
template < uint32_t Depth, uint32_t Width > wide_uint<Depth,Width> operator- (wide_uint<Depth,Width> l, const wide_uint<Depth,Width> &r) { return l -=  r; }
template < uint32_t Depth, uint32_t Width > wide_uint<Depth,Width> operator* (wide_uint<Depth,Width> l, const wide_uint<Depth,Width> &r) { return l *=  r; }
template < uint32_t Depth, uint32_t Width > wide_uint<Depth,Width> operator/ (wide_uint<Depth,Width> l, const wide_uint<Depth,Width> &r) { return l /=  r; }
template < uint32_t Depth, uint32_t Width > wide_uint<Depth,Width> operator% (wide_uint<Depth,Width> l, const wide_uint<Depth,Width> &r) { return l %=  r; }
template < uint32_t Depth, uint32_t Width > wide_uint<Depth,Width> operator<<(wide_uint<Depth,Width> l, const wide_uint<Depth,Width> &r) { return l <<= r; }
template < uint32_t Depth, uint32_t Width > wide_uint<Depth,Width> operator>>(wide_uint<Depth,Width> l, const wide_uint<Depth,Width> &r) { return l >>= r; }

template < uint32_t Depth, uint32_t Width > wide_uint<Depth,Width> operator+ (wide_uint<Depth,Width> l, typename wide_uint<Depth,Width>::serial_t r) { return l +=  r; }
template < uint32_t Depth, uint32_t Width > wide_uint<Depth,Width> operator- (wide_uint<Depth,Width> l, typename wide_uint<Depth,Width>::serial_t r) { return l -=  r; }
template < uint32_t Depth, uint32_t Width > wide_uint<Depth,Width> operator* (wide_uint<Depth,Width> l, typename wide_uint<Depth,Width>::serial_t r) { return l *=  r; }
template < uint32_t Depth, uint32_t Width > wide_uint<Depth,Width> operator/ (wide_uint<Depth,Width> l, typename wide_uint<Depth,Width>::serial_t r) { return l /=  r; }
template < uint32_t Depth, uint32_t Width > wide_uint<Depth,Width> operator% (wide_uint<Depth,Width> l, typename wide_uint<Depth,Width>::serial_t r) { return l %=  r; }
template < uint32_t Depth, uint32_t Width > wide_uint<Depth,Width> operator<<(wide_uint<Depth,Width> l, typename wide_uint<Depth,Width>::serial_t r) { return l <<= r; }
template < uint32_t Depth, uint32_t Width > wide_uint<Depth,Width> operator>>(wide_uint<Depth,Width> l, typename wide_uint<Depth,Width>::serial_t r) { return l >>= r; }

template < uint32_t Depth, uint32_t Width > wide_uint<Depth,Width> operator+ (typename wide_uint<Depth,Width>::serial_t l, const wide_uint<Depth,Width> &r) { return wide_uint<Depth,Width>(l) +=  r; }
template < uint32_t Depth, uint32_t Width > wide_uint<Depth,Width> operator- (typename wide_uint<Depth,Width>::serial_t l, const wide_uint<Depth,Width> &r) { return wide_uint<Depth,Width>(l) -=  r; }
template < uint32_t Depth, uint32_t Width > wide_uint<Depth,Width> operator* (typename wide_uint<Depth,Width>::serial_t l, const wide_uint<Depth,Width> &r) { return wide_uint<Depth,Width>(l) *=  r; }
template < uint32_t Depth, uint32_t Width > wide_uint<Depth,Width> operator/ (typename wide_uint<Depth,Width>::serial_t l, const wide_uint<Depth,Width> &r) { return wide_uint<Depth,Width>(l) /=  r; }
template < uint32_t Depth, uint32_t Width > wide_uint<Depth,Width> operator% (typename wide_uint<Depth,Width>::serial_t l, const wide_uint<Depth,Width> &r) { return wide_uint<Depth,Width>(l) %=  r; }
template < uint32_t Depth, uint32_t Width > wide_uint<Depth,Width> operator<<(typename wide_uint<Depth,Width>::serial_t l, const wide_uint<Depth,Width> &r) { return wide_uint<Depth,Width>(l) <<= r; }
template < uint32_t Depth, uint32_t Width > wide_uint<Depth,Width> operator>>(typename wide_uint<Depth,Width>::serial_t l, const wide_uint<Depth,Width> &r) { return wide_uint<Depth,Width>(l) >>= r; }

template < uint32_t Depth, uint32_t Width >
class alignas(Width * sizeof(typename __wide_types<Depth>::float_t)) wide_float
{
public:
	typedef typename __wide_types<Depth>::float_t serial_t;
	static constexpr uint32_t width = Width;
	static constexpr uint32_t depth = Depth;
	friend class wide_int<Depth,Width>;

private:
	serial_t v[Width];

public:
	wide_float( void ) = default;
	wide_float(const wide_float&) = default;
	wide_float(serial_t r) { FOR(v[i] = r) }
	explicit wide_float(const serial_t *r) { FOR(v[i] = r[i]) }
	explicit wide_float(const wide_int<Depth,Width> &r) { FOR(v[i] = r.v[i]) }
	wide_float &operator=(const wide_float&) = default;
	wide_float &operator=(serial_t r) { FOR(v[i] = r) return *this; }
	wide_float &operator=(const cset<wide_float> &test) { *this = cmov(test.mask, test.a, *this); return *this; }
	wide_float &operator=(const cset<const wide_float> &test) { *this = cmov(test.mask, test.a, *this); return *this; }

	wide_float &operator+=(const wide_float &r) { FOR(v[i] += r.v[i]) return *this; }
	wide_float &operator-=(const wide_float &r) { FOR(v[i] -= r.v[i]) return *this; }
	wide_float &operator*=(const wide_float &r) { FOR(v[i] *= r.v[i]) return *this; }
	wide_float &operator/=(const wide_float &r) { FOR(v[i] /= r.v[i]) return *this; }
	wide_float &operator+=(serial_t r)          { FOR(v[i] +=  r)     return *this; }
	wide_float &operator-=(serial_t r)          { FOR(v[i] -=  r)     return *this; }
	wide_float &operator*=(serial_t r)          { FOR(v[i] *=  r)     return *this; }
	wide_float &operator/=(serial_t r)          { FOR(v[i] /=  r)     return *this; }

	wide_float  operator-( void ) const { wide_float o; FOR(o.v[i] = -v[i]) return o; }
	
	wide_float &operator++( void ) { FOR(++v[i]) return *this; }
	wide_float  operator++( int )  { wide_float o = *this; FOR(++v[i]); return o; }
	wide_float &operator--( void ) { FOR(--v[i]) return *this; }
	wide_float  operator--( int )  { wide_float o = *this; FOR(--v[i]); return o; }

	wide_bool<Depth,Width> operator==(const wide_float &r) const { CMP(==); }
	wide_bool<Depth,Width> operator!=(const wide_float &r) const { CMP(!=); }
	wide_bool<Depth,Width> operator <(const wide_float &r) const { CMP(<); }
	wide_bool<Depth,Width> operator >(const wide_float &r) const { CMP(>); }
	wide_bool<Depth,Width> operator<=(const wide_float &r) const { CMP(<=); }
	wide_bool<Depth,Width> operator>=(const wide_float &r) const { CMP(>=); }
};

template < uint32_t Depth, uint32_t Width > wide_float<Depth,Width> operator+(wide_float<Depth,Width> l, const wide_float<Depth,Width> &r) { return l += r; }
template < uint32_t Depth, uint32_t Width > wide_float<Depth,Width> operator-(wide_float<Depth,Width> l, const wide_float<Depth,Width> &r) { return l -= r; }
template < uint32_t Depth, uint32_t Width > wide_float<Depth,Width> operator*(wide_float<Depth,Width> l, const wide_float<Depth,Width> &r) { return l *= r; }
template < uint32_t Depth, uint32_t Width > wide_float<Depth,Width> operator/(wide_float<Depth,Width> l, const wide_float<Depth,Width> &r) { return l /= r; }
template < uint32_t Depth, uint32_t Width > wide_float<Depth,Width> operator%(wide_float<Depth,Width> l, const wide_float<Depth,Width> &r) { return l %= r; }

template < uint32_t Depth, uint32_t Width > wide_float<Depth,Width> operator+(wide_float<Depth,Width> l, typename wide_float<Depth,Width>::serial_t r) { return l += r; }
template < uint32_t Depth, uint32_t Width > wide_float<Depth,Width> operator-(wide_float<Depth,Width> l, typename wide_float<Depth,Width>::serial_t r) { return l -= r; }
template < uint32_t Depth, uint32_t Width > wide_float<Depth,Width> operator*(wide_float<Depth,Width> l, typename wide_float<Depth,Width>::serial_t r) { return l *= r; }
template < uint32_t Depth, uint32_t Width > wide_float<Depth,Width> operator/(wide_float<Depth,Width> l, typename wide_float<Depth,Width>::serial_t r) { return l /= r; }
template < uint32_t Depth, uint32_t Width > wide_float<Depth,Width> operator%(wide_float<Depth,Width> l, typename wide_float<Depth,Width>::serial_t r) { return l %= r; }

template < uint32_t Depth, uint32_t Width > wide_float<Depth,Width> operator+(typename wide_float<Depth,Width>::serial_t l, const wide_float<Depth,Width> &r) { return wide_float<Depth,Width>(l) += r; }
template < uint32_t Depth, uint32_t Width > wide_float<Depth,Width> operator-(typename wide_float<Depth,Width>::serial_t l, const wide_float<Depth,Width> &r) { return wide_float<Depth,Width>(l) -= r; }
template < uint32_t Depth, uint32_t Width > wide_float<Depth,Width> operator*(typename wide_float<Depth,Width>::serial_t l, const wide_float<Depth,Width> &r) { return wide_float<Depth,Width>(l) *= r; }
template < uint32_t Depth, uint32_t Width > wide_float<Depth,Width> operator/(typename wide_float<Depth,Width>::serial_t l, const wide_float<Depth,Width> &r) { return wide_float<Depth,Width>(l) /= r; }
template < uint32_t Depth, uint32_t Width > wide_float<Depth,Width> operator%(typename wide_float<Depth,Width>::serial_t l, const wide_float<Depth,Width> &r) { return wide_float<Depth,Width>(l) %= r; }

template < typename wide_t >
wide_t cmov(const wide_bool<wide_t::depth, wide_t::width> &condition, const wide_t &a, const wide_t &b) {
	const auto o = (*reinterpret_cast<const wide_bool<wide_t::depth, wide_t::width>*>(&a) & condition) | (*reinterpret_cast<const wide_bool<wide_t::depth, wide_t::width>*>(&b) & (!condition));
	return *reinterpret_cast<const wide_t*>(&o);
}

template < typename wide_t > wide_t *wide_cast(typename wide_t::serial_t *stream, size_t byte_alignment = sizeof(wide_t)) {
	return (stream & ~(byte_alignment - 1)) == stream ? reinterpret_cast<wide_t*>(stream) : nullptr;
}
template < typename wide_t > const wide_t *wide_cast(const typename wide_t::serial_t *stream, size_t byte_alignment = sizeof(wide_t)) {
	return (stream & ~(byte_alignment - 1)) == stream ? reinterpret_cast<wide_t*>(stream) : nullptr;
}

template < typename wide_t > typename wide_t::serial_t       *serialize(wide_t &w)       { return reinterpret_cast<typename wide_t::serial_t*>(&w); }
template < typename wide_t > const typename wide_t::serial_t *serialize(const wide_t &w) { return reinterpret_cast<const typename wide_t::serial_t*>(&w); }

}

#undef FOR
#undef CMP
#undef CMP1
#undef ASSOP
#undef UNIOP
#undef INCOP
#undef CMPOP

#endif //
