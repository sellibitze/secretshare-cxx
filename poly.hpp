#ifndef INC_GUARD_EM7ZIEVDIW81E883BCTXW4Z2K42QIE
#define INC_GUARD_EM7ZIEVDIW81E883BCTXW4Z2K42QIE

#include "gf256.hpp"
#include "slice.hpp"

struct gf256_point
{
	gf256 x, y;
	explicit gf256_point(gf256 x, gf256 y) : x(x), y(y) {}
};

gf256 poly_evaluate(slice<const gf256> polycoeffs, gf256 xcoord);

gf256 poly_interpolate(slice<const gf256_point> points, gf256 xcoord);

#endif /* include guard */

