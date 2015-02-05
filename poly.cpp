#include "poly.hpp"

gf256 poly_evaluate(slice<const gf256> polycoeffs, gf256 xcoord)
{
	auto acc = gf256::zero();
	auto fac = gf256::one();
	while (polycoeffs) {
		auto c = polycoeffs.shift();
		acc += c * fac;
		fac *= xcoord;
	}
	return acc;
}

gf256 poly_interpolate(slice<const gf256_point> points, gf256 xcoord)
{
	// Lagrange interpolation
	const unsigned ord = points.size();
	auto acc = gf256::zero();
	for (unsigned i=0; i<ord; ++i) {
		auto prod = gf256::one();
		auto& pi = points[i];
		for (unsigned j=0; j<ord; ++j) {
			if (i != j) {
				auto& pj = points[j];
				prod *= (xcoord - pj.x) / (pi.x - pj.x);
			}
		}
		acc += prod * pi.y;
	}
	return acc;
}

