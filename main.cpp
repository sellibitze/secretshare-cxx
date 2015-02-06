#include <cassert>
#include <cstddef>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <vector>
#include "slice.hpp"
#include "poly.hpp"
#include "base64.hpp"
#include "getrandom.hpp"

using std::vector;
using std::string;

namespace {

vector<vector<octet>> secret_share(slice<const octet> secret, unsigned k, unsigned n)
{
	auto blocksize = secret.size();
	if (k == 0 || k > 255 || n > 255 || k > n)
		throw std::runtime_error("Illegal K/N parameters");
	vector<octet> poly (k, 0);
	vector<vector<octet>> result (n, vector<octet>(blocksize, octet(0)));
	for (std::size_t byteidx=0; byteidx<blocksize; ++byteidx) {
		poly[0] = secret[byteidx];
		get_random(all_of(poly).slice_from(1));
		for (unsigned shareidx=0; shareidx<n; ++shareidx) {
			result[shareidx][byteidx] = poly_evaluate(poly, shareidx+1);
		}
	}
	return result;
}

} // namespace

int main(int argc, char* argv[])
{
	try {
		static const octet arr[] = {1,2,3,4,5};
		auto shares = secret_share(arr, 2, 4);
	} catch (std::exception const& x) {
		std::cerr << "Exception: " << x.what() << std::endl;
		return EXIT_FAILURE;
	}
}

