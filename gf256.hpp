#ifndef INC_GUARD_3GD04T25G57G53YAHPBG5T2W7D2QYT
#define INC_GUARD_3GD04T25G57G53YAHPBG5T2W7D2QYT

namespace gf {

/// Value type for a finite field with 256 elements
class gf256
{
public:
	explicit gf256(unsigned char v): v(v) {}

	unsigned char to_byte() const { return this->v; }

	static gf256 zero() { return gf256(0); }
	static gf256 one() { return gf256(1); }
	static gf256 exp(unsigned p);

	friend gf256 operator+(gf256 a, gf256 b) { return gf256(a.v ^ b.v); }
	friend gf256 operator-(gf256 a, gf256 b) { return gf256(a.v ^ b.v); }

private:
	unsigned char v;
};

unsigned log(gf256 x);
gf256 operator*(gf256 a, gf256 b);
gf256 operator/(gf256 a, gf256 b);

} // namespace gf

#endif /* include guard */

