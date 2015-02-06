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

struct share
{
	octet xcoord;
	vector<octet> data;
	explicit share(std::size_t bs) : xcoord(0), data(bs) {}
};

vector<share> secret_share(slice<const octet> secret, unsigned k, unsigned n)
{
	auto const blocksize = secret.size();
	if (k == 0 || k > 255 || n > 255 || k > n)
		throw std::runtime_error("Illegal K/N parameters");
	vector<octet> poly (k);
	vector<share> result (n, share(blocksize));
	for (std::size_t byteidx=0; byteidx<blocksize; ++byteidx) {
		get_random(all_of(poly).slice_from(1));
		poly[0] = secret[byteidx];
		for (unsigned shareidx=0; shareidx<n; ++shareidx) {
			auto& s = result[shareidx];
			s.xcoord = shareidx + 1;
			s.data[byteidx] = poly_evaluate(poly, s.xcoord);
		}
	}
	return result;
}

vector<octet> decode_secret(slice<const share> const shares)
{
	assert(!shares.empty());
	auto const k = shares.size();
	auto const blocksize = shares[0].data.size();
	vector<octet> secret (blocksize);
	vector<point> points (k);
	for (std::size_t byteidx=0; byteidx<blocksize; ++byteidx) {
		for (unsigned i=0; i<k; ++i) {
			auto& s = shares[i];
			auto& p = points[i];
			p.x = s.xcoord;
			p.y = s.data[byteidx];
		}
		secret[byteidx] = poly_interpolate(points, 0);
	}
	return secret;
}

} // namespace

int main(int argc, char* argv[])
{
	try {
		static const octet arr[] = {1,2,3,4,5};
		auto shares = secret_share(arr, 2, 4);
		auto secret = decode_secret(shares);
		for (unsigned k=0; k<secret.size(); ++k) {
			std::cout << unsigned(secret[k]) << std::endl;
		}
	} catch (std::exception const& x) {
		std::cerr << "Exception: " << x.what() << std::endl;
		return EXIT_FAILURE;
	}
}

