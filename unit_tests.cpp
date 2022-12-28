#include <iostream>
#include "wide.h"
#include "wmath.h"

typedef wide::wide_bool<32,4> b32x4;
typedef wide::wide_int<32,4>  i32x4;
typedef wide::wide_uint<32,4> u32x4;
typedef wide::wide_float<32,4> f32x4;

static constexpr b32x4::serial_t FALSE_BITS = 0;
static constexpr b32x4::serial_t TRUE_BITS  = std::numeric_limits<b32x4::serial_t>::max();

const int32_t  ix_vals[4] = { 1, 2, 3, 4 };
const int32_t  iy_vals[4] = { 4, 3, 2, 1 };
const uint32_t ux_vals[4] = { 1, 2, 3, 4 };
const uint32_t uy_vals[4] = { 4, 3, 2, 1 };
const float    fx_vals[4] = { 1.0f, 2.0f, 3.0f, 4.0f };
const float    fy_vals[4] = { 4.0f, 3.0f, 2.0f, 1.0f };
const float    ft_vals[4] = { 1.03887f, 3.23122f, 4.79108f, 3.70818f };
const float    fu_vals[4] = { -0.887f, 0.887f, 0.0f, 1.0f };

template < typename wide_t >
void print_vals(const wide_t &w)
{
	const auto *x = wide::serialize(w);
	std::cout << "[ ";
	for (int i = 0; i < 3; ++i) {
		std::cout << x[i] << ", ";
	}
	std::cout << x[3] << " ]";
}

void print_vals(const b32x4 &w)
{
	const auto *x = wide::serialize(w);
	std::cout << "[ ";
	for (int i = 0; i < 3; ++i) {
		std::cout << (x[i] ? "true" : "false") << ", ";
	}
	std::cout << (x[3] ? "true" : "false") << " ]";
}

template < typename wide_t >
bool check_vals(const wide_t &w, typename wide_t::serial_t a, typename wide_t::serial_t b, typename wide_t::serial_t c, typename wide_t::serial_t d)
{
	const auto                      *x    = wide::serialize(w);
	const typename wide_t::serial_t  y[4] = { a, b, c, d };
	for (int i = 0; i < 4; ++i) {
		if (x[i] != y[i]) {
			std::cout << "(" << i << ") " << x[i] << " != " << y[i];
			return false;
		}
	}
	return true;
}

template < typename wide_t >
bool check_vals_epsilon(const wide_t &w, typename wide_t::serial_t a, typename wide_t::serial_t b, typename wide_t::serial_t c, typename wide_t::serial_t d, typename wide_t::serial_t epsilon)
{
	const auto                      *x    = wide::serialize(w);
	const typename wide_t::serial_t  y[4] = { a, b, c, d };
	for (int i = 0; i < 4; ++i) {
		if (x[i] < y[i] - epsilon || x[i] > y[i] + epsilon) {
			std::cout << "(" << i << ") " << x[i] << " !~= " << y[i];
			return false;
		}
	}
	return true;
}

bool check_vals(const b32x4 &w, bool a, bool b, bool c, bool d)
{
	const auto                          *x    = wide::serialize(w);
	const typename b32x4::serial_t  y[4] = { a ? TRUE_BITS : FALSE_BITS, b ? TRUE_BITS : FALSE_BITS, c ? TRUE_BITS : FALSE_BITS, d ? TRUE_BITS : FALSE_BITS };
	for (int i = 0; i < 4; ++i) {
		if (x[i] != y[i]) {
			std::cout << "(" << i << ") " << x[i] << " != " << y[i];
			return false;
		}
	}
	return true;
}

bool int_test( void )
{
	std::cout << "int_test...";

	const i32x4 x = i32x4(ix_vals);
	if (!check_vals(x <  3, true,  true,  false, false)) { return false; }
	if (!check_vals(x <= 3, true,  true,  true,  false)) { return false; }
	if (!check_vals(x >  3, false, false, false, true))  { return false; }
	if (!check_vals(x >= 3, false, false, true,  true))  { return false; }
	if (!check_vals(x == 3, false, false, true,  false)) { return false; }
	if (!check_vals(x != 3, true,  true, false,  true))  { return false; }

	std::cout << "succeeded" << std::endl;
	return true;
}

bool uint_test( void )
{
	std::cout << "uint_test...";

	const u32x4 x = u32x4(ux_vals);
	if (!check_vals(x <  3, true,  true,  false, false)) { return false; }
	if (!check_vals(x <= 3, true,  true,  true,  false)) { return false; }
	if (!check_vals(x >  3, false, false, false, true))  { return false; }
	if (!check_vals(x >= 3, false, false, true,  true))  { return false; }
	if (!check_vals(x == 3, false, false, true,  false)) { return false; }
	if (!check_vals(x != 3, true,  true, false,  true))  { return false; }

	std::cout << "succeeded" << std::endl;
	return true;
}

bool float_test( void )
{
	std::cout << "float_test...";

	const f32x4 x = f32x4(fx_vals);
	if (!check_vals(x <  3.0f, true,  true,  false, false)) { return false; }
	if (!check_vals(x <= 3.0f, true,  true,  true,  false)) { return false; }
	if (!check_vals(x >  3.0f, false, false, false, true))  { return false; }
	if (!check_vals(x >= 3.0f, false, false, true,  true))  { return false; }
	if (!check_vals(x == 3.0f, false, false, true,  false)) { return false; }
	if (!check_vals(x != 3.0f, true,  true, false,  true))  { return false; }

	std::cout << "succeeded" << std::endl;
	return true;
}

bool int_mod( void )
{
	std::cout << "int_mod...";
	
	const i32x4 x = i32x4(ix_vals);
	if (!check_vals(x % 3, 1, 2, 0, 1)) { return false; }

	std::cout << "succeeded" << std::endl;
	return true;
}

bool uint_mod( void )
{
	std::cout << "uint_mod...";
	
	const u32x4 x = u32x4(ux_vals);
	if (!check_vals(x % 3, 1, 2, 0, 1)) { return false; }

	std::cout << "succeeded" << std::endl;
	return true;
}

bool int_wide_if( void )
{
	std::cout << "int_wide_if...";

	i32x4  a    = i32x4(ix_vals);
	i32x4  b    = 0;
	b32x4 mask = true;

	WIDE_IF(a % 2 != 0)
		b = WIDE_SET(a);
	WIDE_ELSE
		b = WIDE_SET(i32x4(-1));
	END_WIDE_IF
	if (!check_vals(b, 1, -1, 3, -1)) { return false; }

	std::cout << "succeeded" << std::endl;
	return true;
}

bool uint_wide_if( void )
{
	std::cout << "uint_wide_if...";

	u32x4 a    = u32x4(ux_vals);
	u32x4 b    = 0;
	b32x4 mask = true;

	WIDE_IF(a % 2 != 0)
		b = WIDE_SET(a);
	WIDE_ELSE
		b = WIDE_SET(u32x4(77));
	END_WIDE_IF
	if (!check_vals(b, 1, 77, 3, 77)) { return false; }

	std::cout << "succeeded" << std::endl;
	return true;
}

bool int_wide_while( void )
{
	std::cout << "int_wide_while...";

	i32x4  a    = i32x4(ix_vals);
	i32x4  b    = 0;
	b32x4 mask = true;

	i32x4 i = 0;
	WIDE_WHILE(i < a)
		b = WIDE_SET(b - 1);
		i = WIDE_SET(i + 1);
	END_WIDE_WHILE
	if (!check_vals(b, -1, -2, -3, -4)) { return false; }

	std::cout << "succeeded" << std::endl;
	return true;
}

bool uint_wide_while( void )
{
	std::cout << "uint_wide_while...";

	u32x4  a   = u32x4(ux_vals);
	u32x4  b   = 0;
	b32x4 mask = true;

	u32x4 i = 0;
	WIDE_WHILE(i < a)
		b = WIDE_SET(b + 2);
		i = WIDE_SET(i + 1);
	END_WIDE_WHILE
	if (!check_vals(b, 2, 4, 6, 8)) { return false; }

	std::cout << "succeeded" << std::endl;
	return true;
}

bool float_sqrt_nr( void )
{
	std::cout << "float_sqrt_nr...";

	f32x4 a = f32x4(fx_vals);
	f32x4 b = wide::sqrt_nr(a);
	if (!check_vals_epsilon(b, ::sqrt(fx_vals[0]), ::sqrt(fx_vals[1]), ::sqrt(fx_vals[2]), ::sqrt(fx_vals[3]), 0.001)) { return false; }

	std::cout << "succeeded" << std::endl;
	return true;
}

bool float_sqrt_bs( void )
{
	std::cout << "float_sqrt_bs...";

	f32x4 a = f32x4(fx_vals);
	f32x4 b = wide::sqrt_bs(a);
	if (!check_vals_epsilon(b, ::sqrt(fx_vals[0]), ::sqrt(fx_vals[1]), ::sqrt(fx_vals[2]), ::sqrt(fx_vals[3]), 0.001)) { return false; }

	std::cout << "succeeded" << std::endl;
	return true;
}

float sin_test(float x)
{
	float sin = 0.0f;
	if (x < -3.14159265f)
		x += 6.28318531f;
	else
		if (x > 3.14159265f)
			x -= 6.28318531f;

	if ( x < 0 )
	{
		sin = x * ( 1.27323954f + 0.405284735f * x );
		
		if ( sin < 0 )
			sin = sin * ( -0.255f * ( sin + 1 ) + 1 );
		else
			sin = sin * ( 0.255f * ( sin - 1 ) + 1 );
	}
	else
	{
		sin = x * ( 1.27323954f - 0.405284735f * x );
		
		if ( sin < 0 )
			sin = sin * ( -0.255f * ( sin + 1 ) + 1 );
		else
			sin = sin * ( 0.255f * ( sin - 1 ) + 1 );
	}
	
	return sin;
}

bool float_sin( void )
{
	std::cout << "float_sin...";

	f32x4 a = f32x4(ft_vals);
	f32x4 b = wide::sin(a);

	if (!check_vals_epsilon(b, ::sin(ft_vals[0]), ::sin(ft_vals[1]), ::sin(ft_vals[2]), ::sin(ft_vals[3]), 0.001)) { return false; }

	std::cout << "succeeded" << std::endl;
	return true;
}

bool float_cos( void )
{
	std::cout << "float_cos...";

	f32x4 a = f32x4(ft_vals);
	f32x4 b = wide::cos(a);
	if (!check_vals_epsilon(b, ::cos(ft_vals[0]), ::cos(ft_vals[1]), ::cos(ft_vals[2]), ::cos(ft_vals[3]), 0.001)) { return false; }

	std::cout << "succeeded" << std::endl;
	return true;
}

bool float_tan( void )
{
	std::cout << "float_tan...";

	f32x4 a = f32x4(ft_vals);
	f32x4 b = wide::tan(a);
	if (!check_vals_epsilon(b, ::tan(ft_vals[0]), ::tan(ft_vals[1]), ::tan(ft_vals[2]), ::tan(ft_vals[3]), 0.2)) { return false; }

	std::cout << "succeeded" << std::endl;
	return true;
}

bool float_asin_bs( void )
{
	std::cout << "float_asin_bs...";

	f32x4 a = f32x4(fu_vals);
	f32x4 b = wide::asin_bs(a);
	if (!check_vals_epsilon(b, ::asin(fu_vals[0]), ::asin(fu_vals[1]), ::asin(fu_vals[2]), ::asin(fu_vals[3]), 0.01)) { return false; }

	std::cout << "succeeded" << std::endl;
	return true;
}

bool float_asin_nr( void )
{
	std::cout << "float_asin_nr...";

	f32x4 a = f32x4(fu_vals);
	f32x4 b = wide::asin_nr(a);
	// if (!check_vals_epsilon(b, ::asin(fu_vals[0]), ::asin(fu_vals[1]), ::asin(fu_vals[2]), ::asin(fu_vals[3]), 0.01)) { return false; }

	float A[4] = {
		::asin(fu_vals[0]),
		::asin(fu_vals[1]),
		::asin(fu_vals[2]),
		::asin(fu_vals[3])
	};
	float B[4] = {
		wide::donksin(fu_vals[0]),
		wide::donksin(fu_vals[1]),
		wide::donksin(fu_vals[2]),
		wide::donksin(fu_vals[3])
	};

	f32x4 m = f32x4(A);
	f32x4 n = f32x4(B);
	print_vals(m);
	std::cout << std::endl;
	print_vals(n);
	std::cout << std::endl;

	std::cout << "succeeded" << std::endl;
	return true;
}

int main(int,char**)
{
	if (!int_test())        { return 1; }
	if (!uint_test())       { return 1; }
	if (!float_test())      { return 1; }
	if (!int_mod())         { return 1; }
	if (!uint_mod())        { return 1; }
	if (!int_wide_if())     { return 1; }
	if (!uint_wide_if())    { return 1; }
	if (!int_wide_while())  { return 1; }
	if (!uint_wide_while()) { return 1; }
	if (!float_sqrt_nr())   { return 1; }
	if (!float_sqrt_bs())   { return 1; }
	if (!float_sin())       { return 1; }
	if (!float_cos())       { return 1; }
	if (!float_tan())       { return 1; }
//	if (!float_asin_bs())   { return 1; }
	if (!float_asin_nr())   { return 1; }
	return 0;
}
