/// @file
/// @author Jonathan Karlsson, github.com/SirJonthe
/// @date 2022, 2023
/// @copyright Jonathan Karlsson
/// @license zlib

#ifndef WIDE_H_INCLUDED__
#define WIDE_H_INCLUDED__

#include <cstdint>
#include <limits>
#include <cstdlib>

#define FOR(x) for (uint32_t i = 0; i < Width; ++i) { x; }
#define CMP(sign)  wide_bool<Depth,Width> o; for (uint32_t i = 0; i < Width; ++i) { o.v[i] = v[i] sign r.v[i] ? wide_bool<Depth,Width>::TRUE_BITS : wide_bool<Depth,Width>::FALSE_BITS; } return o
#define CMP1(sign) wide_bool<Depth,Width> o; for (uint32_t i = 0; i < Width; ++i) { o.v[i] = v[i] sign r      ? wide_bool<Depth,Width>::TRUE_BITS : wide_bool<Depth,Width>::FALSE_BITS; } return o

#define ASSVAL(type) \
	struct values { serial_t vals[Width]; }; \
	type(const values &vals) : type(vals.vals) {} \
	type &operator=(const values &vals) { FOR(v[i] = vals.vals[i]) return *this; }

#define CONSTR(type, from1, from2) \
	type( void ) = default; \
	type(const type&) = default; \
	type(serial_t r) { FOR(v[i] = r) } \
	explicit type(const serial_t *r) { FOR(v[i] = r[i]) } \
	explicit type(const from1<Depth,Width> &r) { FOR(v[i] = serial_t(r.v[i])) } \
	explicit type(const from2<Depth,Width> &r) { FOR(v[i] = serial_t(r.v[i])) } \
	explicit type(const wide_bool<Depth,Width> &r) { FOR(v[i] = r.v[i] ? serial_t(1) : serial_t(0)) } \
	type &operator=(const type&) = default; \
	type &operator=(serial_t r) { FOR(v[i] = r) return *this; } \
	type &operator=(const cset<type> &test) { *this = cmov(test.mask, test.value, *this); return *this; } \
	type &operator=(const cset<const type> &test) { *this = cmov(test.mask, test.value, *this); return *this; }

#define ASSOP(type, op) \
	type &operator op(const type &r) { FOR(v[i] op r.v[i]) return *this; } \
	type &operator op(serial_t r)    { FOR(v[i] op r)      return *this; }

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
	ASSOP(type, %=) \
	ASSOP(type, <<=) \
	ASSOP(type, >>=) \
	ASSOP(type, &=) \
	ASSOP(type, |=) \
	ASSOP(type, ^=) \
	UNIOP(type, ~)

#define CMPOP(type, op) \
	wide_bool<Depth,Width> operator op(const type &r) const     { CMP(op); } \
	wide_bool<Depth,Width> operator op(const serial_t &r) const { CMP1(op); }

#define CMPOPS(type) \
	CMPOP(type, ==) \
	CMPOP(type, !=) \
	CMPOP(type, <) \
	CMPOP(type, >) \
	CMPOP(type, <=) \
	CMPOP(type, >=)

#define OPOP(type, op) \
	template < uint32_t Depth, uint32_t Width > type<Depth,Width> operator op(type<Depth,Width> l, const type<Depth,Width> &r)                           { return l op##= r; } \
	template < uint32_t Depth, uint32_t Width > type<Depth,Width> operator op(type<Depth,Width> l, const typename type<Depth,Width>::serial_t &r)        { return l op##= r; } \
	template < uint32_t Depth, uint32_t Width > type<Depth,Width> operator op(const typename type<Depth,Width>::serial_t &l, const type<Depth,Width> &r) { return type<Depth,Width>(l) op##= r; }

#define ARITOPOPS(type) \
	OPOP(type, +) \
	OPOP(type, -) \
	OPOP(type, *) \
	OPOP(type, /)

#define INTARITOPOPS(type) \
	ARITOPOPS(type) \
	OPOP(type, %) \
	OPOP(type, <<) \
	OPOP(type, >>) \
	OPOP(type, &) \
	OPOP(type, |) \
	OPOP(type, ^)

#define CMPOPOPS(type) \
	template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> operator ==(const typename type<Depth,Width>::serial_t &l, const type<Depth,Width> &r) { return r == l; } \
	template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> operator !=(const typename type<Depth,Width>::serial_t &l, const type<Depth,Width> &r) { return r != l; } \
	template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> operator < (const typename type<Depth,Width>::serial_t &l, const type<Depth,Width> &r) { return r >= l; } \
	template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> operator > (const typename type<Depth,Width>::serial_t &l, const type<Depth,Width> &r) { return r <= l; } \
	template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> operator <=(const typename type<Depth,Width>::serial_t &l, const type<Depth,Width> &r) { return r >  l; } \
	template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> operator >=(const typename type<Depth,Width>::serial_t &l, const type<Depth,Width> &r) { return r <  l; }


/// Establishes a conditional block where the code inside the block is only executed when a lane inside the wide condition is true.
///
/// @note Remember to set up a boolean variable named 'mask' at the root of the function, either as an input parameter or as a local define (all values usually set to all-true).
/// @note Use WIDE_SET to set the value of variables not defined within the current scope.
///
/// @param condition the condition where evaluation to true executes the contents of the block.
///
/// @sa WIDE_ELSE
/// @sa END_WIDE_IF
/// @sa WIDE_SET
#define WIDE_IF(condition) \
	{ \
		const auto &mask0 = mask; \
		auto mask = (condition) & mask0; \
		if (bool(mask)) {


/// Helps setting up a conditional statement where the condition itself is a wide boolean type.
///
/// @note WIDE_ELSE must follow use of WIDE_IF.
/// @note Use WIDE_SET to set the value of variables not defined within the current scope.
///
/// @sa WIDE_IF
/// @sa END_WIDE_IF
/// @sa WIDE_SET
#define WIDE_ELSE \
		} \
		mask = !mask & mask0; \
		if (bool(mask)) {


/// Concludes a wide conditional statement.
///
/// @sa WIDE_IF
/// @sa WIDE_ELSE
#define END_WIDE_IF \
		} \
	}


/// Establishes a conditional block where the code inside the block is only executed when a lane inside the wide condition is true. Repeats the condition until all lanes evaluate to false, although masks out calculations in lanes evaluating to false as long as WIDE_SET is used.
///
/// @note Remember to set up a boolean variable named 'mask' at the root of the function, either as an input parameter or as a local define (all values usually set to all-true).
/// @note Use WIDE_SET to set the value of variables not defined within the current scope.
///
/// @param condition the condition where evaluation to true executes the contents of the block until evaluating to false.
///
/// @sa END_WIDE_WHILE
/// @sa WIDE_SET
#define WIDE_WHILE(condition) \
	{ \
		const auto &mask0 = mask; \
		do { \
			auto mask = (condition) & mask0; \
			if (bool(mask)) {


/// Concludes a wide conditional while statement.
///
/// @sa WIDE_WHILE
#define END_WIDE_WHILE \
			} else { \
				break; \
			} \
		} while (true); \
	}


/// Establishes a conditional block where the code inside the block is executed once, then subsequently only executed when a lane inside the wide condition is true. Repeats the condition until all lanes evaluate to false, although masks out calculations in lanes evaluating to false as long as WIDE_SET is used.
///
/// @note Remember to set up a boolean variable named 'mask' at the root of the function, either as an input parameter or as a local define (all values usually set to all-true).
/// @note Use WIDE_SET to set the value of variables not defined within the current scope.
///
/// @sa END_WIDE_DOWHILE
/// @sa WIDE_SET
#define WIDE_DOWHILE \
	{ \
		const auto &mask0 = mask; \
		auto mask = mask0; \
		do { \
			if (bool(mask)) {


/// Concludes a conditional block where the code inside the block is only executed when a lane inside the wide condition is true. Repeats the condition until all lanes evaluate to false, although masks out calculations in lanes evaluating to false as long as WIDE_SET is used.
///
/// @param condition the condition where evaluation to true executes the contents of the block until evaluating to false.
///
/// @sa WIDE_DOWHILE
#define END_WIDE_DOWHILE(condition) \
			} else { \
				break; \
			} \
			mask = (condition) & mask0; \
		} while (true); \
	}


/// Helps modifying values defined outside the current conditional scope.
///
/// @note Remember to set up a boolean variable named 'mask' at the root of the function, either as an input parameter or as a local define (all values usually set to all-true).
/// @note Use WIDE_SET to set the value of variables not defined within the current conditional scope.
///
/// @sa WIDE_IF
/// @sa WIDE_ELSE
/// @sa WIDE_WHILE
/// @sa WIDE_DOWHILE
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


/// A data structure allowing for conditional set based off of the input mask. The value is set when the mask represents a TRUE state. This, or cmov, is necessary to use inside wide conditionals.
///
/// @note Remember to set up a boolean variable named 'mask' at the root of the function, either as an input parameter or as a local define (all values usually set to all-true).
/// @note Use WIDE_SET instead for ease of use.
///
/// @param condition the condition where evaluation to true executes the contents of the block.
///
/// @sa WIDE_SET
template < typename wide_t >
struct cset
{
	const wide_bool<wide_t::depth,wide_t::width> &mask;
	const wide_t                                  value;
};


/// A data type representing a number of boolean values at a given bit depth. The type is meant to be used as a single value, so all operations are component-wise and the elements of the type can not be accessed directly.
///
/// @sa wide_int
/// @sa wide_uint
/// @sa wide_float
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
	
	explicit wide_bool(const bool *r)                    { FOR(v[i] = r[i]   ? TRUE_BITS : FALSE_BITS) }
	explicit wide_bool(const wide_int<Depth,Width> &r)   { FOR(v[i] = r.v[i] ? TRUE_BITS : FALSE_BITS) }
	explicit wide_bool(const wide_uint<Depth,Width> &r)  { FOR(v[i] = r.v[i] ? TRUE_BITS : FALSE_BITS) }
	explicit wide_bool(const wide_float<Depth,Width> &r) { FOR(v[i] = r.v[i] ? TRUE_BITS : FALSE_BITS) }
	
	wide_bool &operator=(const wide_bool&) = default;
	wide_bool &operator=(bool r)                            { FOR(v[i] = r ? TRUE_BITS : FALSE_BITS) return *this; }
	wide_bool &operator=(const cset<wide_bool> &test)       { *this = cmov(test.mask, test.value, *this); return *this; }
	wide_bool &operator=(const cset<const wide_bool> &test) { *this = cmov(test.mask, test.value, *this); return *this; }

	struct values { serial_t vals[Width]; };
	wide_bool(const values &vals) : wide_bool(vals.vals) {}
	wide_bool &operator=(const values &vals) { FOR(v[i] = vals.vals[i]) return *this; }

	wide_bool &operator &=(const wide_bool &r) { FOR(v[i] &= r.v[i]) return *this; }
	wide_bool &operator &=(bool r)             { FOR(v[i] &= r)      return *this; }
	wide_bool &operator |=(const wide_bool &r) { FOR(v[i] |= r.v[i]) return *this; }
	wide_bool &operator |=(bool r)             { FOR(v[i] |= r)      return *this; }
	wide_bool &operator ^=(const wide_bool &r) { FOR(v[i] ^= r.v[i]) return *this; }
	wide_bool &operator ^=(bool r)             { FOR(v[i] ^= r)      return *this; }
	
	wide_bool operator!( void ) const { wide_bool o; FOR(o.v[i] = ~v[i]) return o; }

	CMPOPS(wide_bool)
	wide_bool operator&&(const wide_bool &r) const { return (*this) & r; }
	wide_bool operator&&(bool r) const             { return (*this) & r; }
	wide_bool operator||(const wide_bool &r) const { return (*this) | r; }
	wide_bool operator||(bool r) const             { return (*this) | r; }

	operator bool( void ) const { serial_t o = 0; FOR(o |= v[i]) return o ? true : false; }

	static wide_bool wide_true( void )  { return wide_bool(true); }
	static wide_bool wide_false( void ) { return wide_bool(false); }
};

template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> TRUE( void ) { return wide_bool<Depth,Width>(true); }
template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> FALSE( void ) { return wide_bool<Depth,Width>(false); }

template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> operator &(wide_bool<Depth,Width> l, const wide_bool<Depth,Width> &r) { return l &= r; }
template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> operator &(wide_bool<Depth,Width> l, bool r)                          { return l &= r; }
template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> operator &(bool l,                   const wide_bool<Depth,Width> &r) { return wide_bool<Depth,Width>(l) &= r; }
template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> operator |(wide_bool<Depth,Width> l, const wide_bool<Depth,Width> &r) { return l |= r; }
template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> operator |(wide_bool<Depth,Width> l, bool r)                          { return l |= r; }
template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> operator |(bool l,                   const wide_bool<Depth,Width> &r) { return wide_bool<Depth,Width>(l) |= r; }
template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> operator ^(wide_bool<Depth,Width> l, const wide_bool<Depth,Width> &r) { return l ^= r; }
template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> operator ^(wide_bool<Depth,Width> l, bool r)                          { return l ^= r; }
template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> operator ^(bool l,                   const wide_bool<Depth,Width> &r) { return wide_bool<Depth,Width>(l) ^= r; }

template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> operator ==(bool l, const wide_bool<Depth,Width> &r) { return wide_bool<Depth,Width>(l) == r; }
template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> operator !=(bool l, const wide_bool<Depth,Width> &r) { return wide_bool<Depth,Width>(l) != r; }
template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> operator < (bool l, const wide_bool<Depth,Width> &r) { return wide_bool<Depth,Width>(l) >= r; }
template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> operator > (bool l, const wide_bool<Depth,Width> &r) { return wide_bool<Depth,Width>(l) <= r; }
template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> operator <=(bool l, const wide_bool<Depth,Width> &r) { return wide_bool<Depth,Width>(l) >  r; }
template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> operator >=(bool l, const wide_bool<Depth,Width> &r) { return wide_bool<Depth,Width>(l) <  r; }

template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> operator &&(bool l, const wide_bool<Depth,Width> &r) { return wide_bool<Depth,Width>(l) && r; }
template < uint32_t Depth, uint32_t Width > wide_bool<Depth,Width> operator ||(bool l, const wide_bool<Depth,Width> &r) { return wide_bool<Depth,Width>(l) && r; }


/// A data type representing a number of signed integer values at a given bit depth. The type is meant to be used as a single value, so all operations are component-wise and the elements of the type can not be accessed directly.
///
/// @sa wide_bool
/// @sa wide_uint
/// @sa wide_float
template < uint32_t Depth, uint32_t Width >
class alignas(Width * sizeof(typename __wide_types<Depth>::int_t)) wide_int
{
public:
	typedef typename __wide_types<Depth>::int_t serial_t;
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
	CMPOPS(wide_int)
};
INTARITOPOPS(wide_int)
CMPOPOPS(wide_int)


/// A data type representing a number of unsigned integer values at a given bit depth. The type is meant to be used as a single value, so all operations are component-wise and the elements of the type can not be accessed directly.
///
/// @sa wide_bool
/// @sa wide_int
/// @sa wide_float
template < uint32_t Depth, uint32_t Width >
class alignas(Width * sizeof(typename __wide_types<Depth>::uint_t)) wide_uint
{
public:
	typedef typename __wide_types<Depth>::uint_t serial_t;
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
	CMPOPS(wide_uint)
};
INTARITOPOPS(wide_uint)
CMPOPOPS(wide_uint)



/// A data type representing a number of signed floating point values at a given bit depth. The type is meant to be used as a single value, so all operations are component-wise and the elements of the type can not be accessed directly.
///
/// @sa wide_bool
/// @sa wide_int
/// @sa wide_uint
template < uint32_t Depth, uint32_t Width >
class alignas(Width * sizeof(typename __wide_types<Depth>::float_t)) wide_float
{
public:
	typedef typename __wide_types<Depth>::float_t serial_t;
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
ARITOPOPS(wide_float)
CMPOPOPS(wide_float)


/// Stores 'a' when condition is true, and 'b' when condition is false for each lane in the wide values. This function is used to merge the results of branching paths together into a single wide value.
///
/// @param condition the condition for which to merge 'a' and 'b' into a single output.
/// @param a a wide value.
/// @param b a wide value.
///
/// @returns the merged results between 'a' and 'b'; lane 'a' when corresponding lane in 'condition' is true, and lane 'b' otherwise.
///
/// @sa wide_cset
template < typename wide_t >
wide_t cmov(const wide_bool<wide_t::depth, wide_t::width> &condition, const wide_t &a, const wide_t &b) {
	const auto o = (*reinterpret_cast<const wide_bool<wide_t::depth, wide_t::width>*>(&a) & condition) | (*reinterpret_cast<const wide_bool<wide_t::depth, wide_t::width>*>(&b) & (!condition));
	return *reinterpret_cast<const wide_t*>(&o);
}


/// Stores 'a' when condition is true, and 'b' when condition is false for each lane in the wide values. This function is used to merge the results of branching paths together into a single wide value.
///
/// @param condition the condition for which to merge 'a' and 'b' into a single output.
/// @param a a serial value.
/// @param b a wide value.
///
/// @returns the merged results between 'a' and 'b'; lane 'a' when corresponding lane in 'condition' is true, and lane 'b' otherwise.
///
/// @sa wide_cset
template < typename wide_t > wide_t cmov(const wide_bool<wide_t::depth, wide_t::width> &condition, typename wide_t::serial_t a, const wide_t &b) { return cmov(condition, wide_t(a), b); }


/// Stores 'a' when condition is true, and 'b' when condition is false for each lane in the wide values. This function is used to merge the results of branching paths together into a single wide value.
///
/// @param condition the condition for which to merge 'a' and 'b' into a single output.
/// @param a a wide value.
/// @param b a serial value.
///
/// @returns the merged results between 'a' and 'b'; lane 'a' when corresponding lane in 'condition' is true, and lane 'b' otherwise.
///
/// @sa wide_cset
template < typename wide_t > wide_t cmov(const wide_bool<wide_t::depth, wide_t::width> &condition, const wide_t &a, typename wide_t::serial_t b) { return cmov(condition, a, wide_t(b)); }


/// Directly converts pointer to a raw array of basic built-in types into a pointer to a wide type array with an optional memory alignment requirement which defaults to the byte size of the target wide type.
///
/// @note It is recommended to ensure byte alignment requirements with the input array, as otherwise this may cause the resulting wide type pointer to point futher ahead than the input serial pointer.
///
/// @note Some architectures may not be able to convert non-aligned input memory. Others may impose performance penalties for operating on non-aligned output memory.
///
/// @param stream pointer to the array of serial values to convert to wide values.
/// @param byte_alignment the number of bytes to align the output memory to. Defaults to the byte count of the wide type, as that is generally safe. If the input memory is not already aligned the output memory will point ahead of the input memory.
///
/// @returns pointer to the array of wide values.
template < typename wide_t > wide_t *wide_cast(typename wide_t::serial_t *stream, size_t byte_alignment = sizeof(wide_t)) { return (stream & ~(byte_alignment - 1)) == stream ? reinterpret_cast<wide_t*>(stream) : nullptr; }


/// Directly converts pointer to a raw array of basic built-in types into a pointer to a wide type array with an optional memory alignment requirement which defaults to the byte size of the target wide type.
///
/// @note It is recommended to ensure byte alignment requirements with the input array, as otherwise this may cause the resulting wide type pointer to point futher ahead than the input serial pointer.
///
/// @note Some architectures may not be able to convert non-aligned input memory. Others may impose performance penalties for operating on non-aligned output memory.
///
/// @param stream pointer to the array of serial values to convert to wide values.
/// @param byte_alignment the number of bytes to align the output memory to. Defaults to the byte count of the wide type, as that is generally safe. If the input memory is not already aligned the output memory will point ahead of the input memory.
///
/// @returns pointer to the array of wide values.
template < typename wide_t > const wide_t *wide_cast(const typename wide_t::serial_t *stream, size_t byte_alignment = sizeof(wide_t)) { return (stream & ~(byte_alignment - 1)) == stream ? reinterpret_cast<wide_t*>(stream) : nullptr; }


/// Casts a wide type into its serial components in order to be able to directly access the internal types of the wide type. This is not generally recommended unless you are flushing data out to a serial array.
///
/// @param w the input wide type to access.
///
/// @returns pointer to the array of serial values that the input wide value is composed of.
template < typename wide_t > typename wide_t::serial_t *serialize(wide_t &w) { return reinterpret_cast<typename wide_t::serial_t*>(&w); }


/// Casts a wide type into its serial components in order to be able to directly access the internal types of the wide type. This is not generally recommended unless you are flushing data out to a serial array.
///
/// @param w the input wide type to access.
///
/// @returns pointer to the array of serial values that the input wide value is composed of.
template < typename wide_t > const typename wide_t::serial_t *serialize(const wide_t &w) { return reinterpret_cast<const typename wide_t::serial_t*>(&w); }

// Useful typedefs. Provide WIDE_DEPTH and WIDE_WIDTH defines through the build stage.
#if defined(WIDE_DEPTH) && defined(WIDE_WIDTH)
	#define WIDE_DEFAULTS
	typedef wide_bool<WIDE_DEPTH,WIDE_WIDTH>  bool_t;
	typedef wide_int<WIDE_DEPTH,WIDE_WIDTH>   int_t;
	typedef wide_uint<WIDE_DEPTH,WIDE_WIDTH>  uint_t;
	typedef wide_float<WIDE_DEPTH,WIDE_WIDTH> float_t;
#endif

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
#undef CMPOP
#undef CMPOPS
#undef OPOP
#undef ARITOPOPS
#undef INTARITOPOPS
#undef CMPOPOPS

#endif // WIDE_H_INCLUDED__
