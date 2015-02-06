#ifndef INC_GUARD_EM7ZIEVDIW81E883BCTXW4Z2K42QIE
#define INC_GUARD_EM7ZIEVDIW81E883BCTXW4Z2K42QIE

#include "slice.hpp"

typedef unsigned char octet;

struct point
{
	octet x, y;
	explicit point(octet x, octet y) : x(x), y(y) {}
	point() : x(0), y(0) {}
};

octet poly_evaluate(slice<const octet> polycoeffs, octet xcoord);

octet poly_interpolate(slice<const point> points, octet xcoord);

#endif /* include guard */

