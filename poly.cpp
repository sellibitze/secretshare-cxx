#include "gf256.hpp"
#include "poly.hpp"

octet poly_evaluate(slice<const octet> polycoeffs, octet xcoord)
{
	auto const x = gf256(xcoord);
	auto acc = gf256::zero();
	auto fac = gf256::one();
	while (polycoeffs) {
		auto c = gf256(polycoeffs.shift());
		acc += c * fac;
		fac *= x;
	}
	return acc.to_byte();
}

octet poly_interpolate(slice<const point> points, octet xcoord)
{
	// Lagrange interpolation
	const unsigned ord = points.size();
	const auto x = gf256(xcoord);
	auto acc = gf256::zero();
	for (unsigned i=0; i<ord; ++i) {
		auto prod = gf256::one();
		auto& pi = points[i];
		for (unsigned j=0; j<ord; ++j) {
			if (i != j) {
				auto& pj = points[j];
				prod *= (x - gf256(pj.x)) / (gf256(pi.x) - gf256(pj.x));
			}
		}
		acc += prod * gf256(pi.y);
	}
	return acc.to_byte();
}

