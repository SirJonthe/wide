#ifndef WIDE_H_INCLUDED__
#define WIDE_H_INCLUDED__

#include <cstdint>
#include <limits>
#include <cstdlib>

#define FOR(x) for (uint32_t i = 0; i < Width; ++i) { x; }
#define CMP(sign) wide_bool<Depth,Width> o; for (uint32_t i = 0; i < Width; ++i) { o.v[i] = v[i] sign r.v[i] ? wide_bool<Depth,Width>::TRUE_BITS : wide_bool<Depth,Width>::FALSE_BITS; } return o
#define CMP1(sign) wide_bool<Depth,Width> o; for (uint32_t i = 0; i < Width; ++i) { o.v[i] = v[i] sign r ? wide_bool<Depth,Width>::TRUE_BITS : wide_bool<Depth,Width>::FALSE_BITS; } return o

#define ASSVAL(type) \
	struct values \
	{ \
		external_t vals[Width]; \
	}; \
	type(const values &vals) { FOR(v[i] = vals.vals[i]) } \
	type &operator=(const values &vals) { FOR(v[i] = vals.vals[i]) return *this; }

#define CONSTR(type, from1, from2) \
	type( void ) = default; \
	type(const type&) = default; \
	type(external_t r) { FOR(v[i] = r) } \
	explicit type(const external_t *r) { FOR(v[i] = r[i]) } \
	explicit type(const from1<Depth,Width> &r) { FOR(v[i] = r.v[i]) } \
	explicit type(const from2<Depth,Width> &r) { FOR(v[i] = r.v[i]) } \
	explicit type(const wide_bool<Depth,Width> &r)  { FOR(v[i] = r.v[i] ? serial_t(1) : serial_t(0)) } \
	type &operator=(const type&) = default; \
	type &operator=(external_t r) { FOR(v[i] = r) return *this; } \
	type &operator=(const cset<type> &test) { *this = cmov(test.mask, test.a, *this); return *this; } \
	type &operator=(const cset<const type> &test) { *this = cmov(test.mask, test.a, *this); return *this; }

#define ASSOP(type, op) \
	type &operator op(const type &r) { FOR(v[i] op r.v[i]) return *this; } \
	type &operator op(external_t r)  { FOR(v[i] op r)      return *this; }

#define INCOP(type, op) \
	type &operator op( void ) { FOR(op v[i]) return *this; } \
	type  operator op( int )  { type o = *this; FOR(op v[i]); return o; }

#define UNIOP(type, op) \
	type operator op( void ) const { type o; FOR(o.v[i] = op v[i]) return o; }

#define ARITASSOPS(type) \
	ASSOP(type, +=) \
	ASSOP(type, -=) \
	ASSOP(type, *=) \
	ASSOP(type, /=) \
	INCOP(type, ++) \
	INCOP(type, --)

#define INTARITASSOPS(type) \
	ARITASSOPS(type) \
	ASSOP(type, %=)

#define BITOPS(type) \
	ASSOP(type, <<=) \
	ASSOP(type, >>=) \
	ASSOP(type, &=) \
	ASSOP(type, |=) \
	ASSOP(type, ^=) \
	UNIOP(type, ~)

#define CMPOP(type, op) \
	wide_bool<Depth,Width> operator op(const type &r) const { CMP(op); }

#define CMPOPS(type) \
	CMPOP(type, ==) \
	CMPOP(type, !=) \
	CMPOP(type, <) \
	CMPOP(type, >) \
	CMPOP(type, <=) \
	CMPOP(type, >=)

#define OPOP(type, op) \
	template < uint32_t Depth, uint32_t Width > type<Depth,Width> operator op(type<Depth,Width> l, const type<Depth,Width> &r)                             { return l op##= r; } \
	template < uint32_t Depth, uint32_t Width > type<Depth,Width> operator op(type<Depth,Width> l, const typename type<Depth,Width>::external_t &r)        { return l op##= r; } \
	template < uint32_t Depth, uint32_t Width > type<Depth,Width> operator op(const typename type<Depth,Width>::external_t &l, const type<Depth,Width> &r) { return type<Depth,Width>(l) op##= r; }

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

template < int bits >
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
	typedef bool external_t;
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
	wide_bool(external_t r) { FOR(v[i] = r ? TRUE_BITS : FALSE_BITS) }
	
	explicit wide_bool(const external_t *r) { FOR(v[i] = r[i] ? TRUE_BITS : FALSE_BITS) }
	explicit wide_bool(const wide_int<Depth,Width> &r) { FOR(v[i] = r.v[i] ? TRUE_BITS : FALSE_BITS); }
	
	wide_bool &operator=(const wide_bool&) = default;
	wide_bool &operator=(external_t r) { FOR(v[i] = r ? TRUE_BITS : FALSE_BITS) return *this; }
	wide_bool &operator=(const cset<wide_bool> &test) { *this = cmov(test.mask, test.a, *this); return *this; }
	wide_bool &operator=(const cset<const wide_bool> &test) { *this = cmov(test.mask, test.a, *this); return *this; }

	ASSVAL(wide_bool)

	ASSOP(wide_bool, &=)
	ASSOP(wide_bool, |=)
	ASSOP(wide_bool, ^=)
	wide_bool operator!( void ) const { wide_bool o; FOR(o.v[i] = ~v[i]) return o; }

	CMPOPS(wide_bool)
	wide_bool operator&&(const wide_bool &r) const { return (*this) & r; }
	wide_bool operator||(const wide_bool &r) const { return (*this) | r; }

	operator bool( void ) const { serial_t o = 0; FOR(o |= v[i]) return o ? true : false; }

	static wide_bool wide_true( void )  { return wide_bool(true); }
	static wide_bool wide_false( void ) { return wide_bool(false); }
};

template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> TRUE( void ) { return wide_bool<Depth,Width>(true); }
template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> FALSE( void ) { return wide_bool<Depth,Width>(false); }

OPOP(wide_bool, &)
OPOP(wide_bool, |)
OPOP(wide_bool, ^)

template < uint32_t Depth, uint32_t Width >
class alignas(Width * sizeof(typename __wide_types<Depth>::int_t)) wide_int
{
public:
	typedef typename __wide_types<Depth>::int_t serial_t;
	typedef serial_t external_t;
	static constexpr uint32_t width = Width;
	static constexpr uint32_t depth = Depth;
	friend class wide_float<Depth,Width>;
	friend class wide_uint<Depth,Width>;

private:
	serial_t v[Width];

public:
	CONSTR(wide_int, wide_uint, wide_float)
	ASSVAL(wide_int)
	INTARITASSOPS(wide_int)
	UNIOP(wide_int, -)
	BITOPS(wide_int)
	CMPOPS(wide_int)
};

OPOP(wide_int, +)
OPOP(wide_int, -)
OPOP(wide_int, *)
OPOP(wide_int, /)
OPOP(wide_int, %)
OPOP(wide_int, <<)
OPOP(wide_int, >>)

template < uint32_t Depth, uint32_t Width >
class alignas(Width * sizeof(typename __wide_types<Depth>::uint_t)) wide_uint
{
public:
	typedef typename __wide_types<Depth>::uint_t serial_t;
	typedef serial_t external_t;
	static constexpr uint32_t width = Width;
	static constexpr uint32_t depth = Depth;
	friend class wide_float<Depth,Width>;
	friend class wide_int<Depth,Width>;

private:
	serial_t v[Width];

public:
	CONSTR(wide_uint, wide_float, wide_int)
	ASSVAL(wide_uint)
	INTARITASSOPS(wide_uint)
	BITOPS(wide_uint)
	CMPOPS(wide_uint)
};

OPOP(wide_uint, +)
OPOP(wide_uint, -)
OPOP(wide_uint, *)
OPOP(wide_uint, /)
OPOP(wide_uint, %)
OPOP(wide_uint, <<)
OPOP(wide_uint, >>)
OPOP(wide_uint, &)
OPOP(wide_uint, |)
OPOP(wide_uint, ^)

template < uint32_t Depth, uint32_t Width >
class alignas(Width * sizeof(typename __wide_types<Depth>::float_t)) wide_float
{
public:
	typedef typename __wide_types<Depth>::float_t serial_t;
	typedef serial_t external_t;
	static constexpr uint32_t width = Width;
	static constexpr uint32_t depth = Depth;
	friend class wide_int<Depth,Width>;
	friend class wide_uint<Depth,Width>;

private:
	serial_t v[Width];

public:
	CONSTR(wide_float, wide_int, wide_uint)
	ASSVAL(wide_float)
	ARITASSOPS(wide_float)
	UNIOP(wide_float, -)
	CMPOPS(wide_float)
};

OPOP(wide_float, +)
OPOP(wide_float, -)
OPOP(wide_float, *)
OPOP(wide_float, /)

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
#undef ASSVAL
#undef CONSTR
#undef ASSOP
#undef INCOP
#undef UNIOP
#undef ARITASSOPS
#undef INTARITASSOPS
#undef BITOPS
#undef CMPOP
#undef CMPOPS

#endif // WIDE_H_INCLUDED__
