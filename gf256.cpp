#include <stdexcept>
#include "gf256.hpp"

namespace {

const unsigned REDUCE_POLY = 0x11D; // => x^8 + x^4 + x^3 + x^2 + 1

/// multiplies an element of the field with a generator `g`
/// of the multiplicative group. In this case: the polynomial `x`
inline unsigned mul_g(unsigned v)
{
	return (v << 1) ^ ((-(v >> 7)) & REDUCE_POLY);
}

struct tables
{
	unsigned char log[256];
	unsigned char exp[256];

	tables();
};

tables::tables()
{
	this->log[0] = 0; // invalid dummy
	for (unsigned p=0, f=1; p<255; ++p) {
		this->exp[p] = f;
		this->log[f] = p;
		f = mul_g(f);
	}
	this->exp[255] = 1;
}

tables const& get_tables()
{
	static tables t;
	return t;
}

} // namespace

namespace gf {

gf256 gf256::exp(unsigned p)
{
	return gf256(get_tables().exp[p % 255]);
}

unsigned log(gf256 x)
{
	return get_tables().log[x.to_byte()];
}

gf256 operator*(gf256 a, gf256 b)
{
	unsigned raw_a = a.to_byte();
	unsigned raw_b = b.to_byte();
	if (raw_a == 0 || raw_b == 0) {
		return gf256::zero();
	} else {
		auto& t = get_tables();
		return gf256(t.exp[(t.log[raw_a] + t.log[raw_b]) % 255]);
	}
}

gf256 operator/(gf256 a, gf256 b)
{
	unsigned raw_a = a.to_byte();
	unsigned raw_b = b.to_byte();
	if (raw_a == 0) {
		return gf256::zero();
	} else if (raw_b == 0) {
		throw std::domain_error("division by zero");
	} else {
		auto& t = get_tables();
		return gf256(t.exp[(t.log[raw_a] + 255 - t.log[raw_b]) % 255]);
	}
}

} // namespace gf

