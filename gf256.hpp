#ifndef INC_GUARD_3GD04T25G57G53YAHPBG5T2W7D2QYT
#define INC_GUARD_3GD04T25G57G53YAHPBG5T2W7D2QYT

/// Value type for a finite field with 256 elements
class gf256
{
public:
	explicit gf256(unsigned v): v(v & 0xFF) {}

	unsigned to_byte() const { return this->v; }

	static gf256 zero() { return gf256(0); }
	static gf256 one() { return gf256(1); }
	static gf256 exp(unsigned p);

	friend gf256 operator+(gf256 a, gf256 b) { return gf256(a.v ^ b.v); }
	friend gf256 operator-(gf256 a, gf256 b) { return gf256(a.v ^ b.v); }
	friend gf256& operator+=(gf256& a, gf256 b) { a.v ^= b.v; return a; }
	friend gf256& operator-=(gf256& a, gf256 b) { a.v ^= b.v; return a; }

private:
	unsigned char v;
};

unsigned log(gf256 x);
gf256 operator*(gf256 a, gf256 b);
gf256 operator/(gf256 a, gf256 b);

inline gf256 operator-(gf256 x) { return x; }
inline gf256& operator*=(gf256& a, gf256 b) { return a = a * b; }
inline gf256& operator/=(gf256& a, gf256 b) { return a = a / b; }

#endif /* include guard */

