#include <cassert>
#include <cstddef>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>
#include <cstdint>
#include "crc24.hpp"
#include "slice.hpp"
#include "poly.hpp"
#include "base64.hpp"
#include "getrandom.hpp"

namespace {

using std::vector;
using std::string;
using std::ostringstream;
using std::size_t;
using std::uint32_t;
using std::runtime_error;

struct share
{
	octet xcoord;
	vector<octet> data;
	explicit share(size_t bs) : xcoord(0), data(bs) {}
};

vector<share> secret_share(slice<const octet> const secret, unsigned k, unsigned n)
{
	auto const blocksize = secret.size();
	if (k == 0 || k > 255 || n > 255 || k > n)
		throw runtime_error("Illegal K/N parameters");
	vector<octet> poly (k);
	vector<share> result (n, share(blocksize));
	for (size_t byteidx=0; byteidx<blocksize; ++byteidx) {
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
	for (size_t byteidx=0; byteidx<blocksize; ++byteidx) {
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

uint32_t compute_crc24(unsigned k, unsigned n, slice<const octet> const data)
{
	crc24hasher h;
	h.process_byte(k);
	h.process_byte(n);
	h.process_block(data);
	return h.finish();
}

string crc24_to_text(uint32_t crc24)
{
	const octet bytes[] = {
		static_cast<octet>((crc24 >> 16) & 0xFF),
		static_cast<octet>((crc24 >>  8) & 0xFF),
		static_cast<octet>((crc24      ) & 0xFF)
	};
	return base64_enc(bytes);
}

string share_to_text(unsigned k, share const& s, bool with_checksum)
{
	ostringstream oss;
	unsigned n = s.xcoord;
	oss << k << '-' << n << '-' << base64_enc(s.data);
	if (with_checksum) {
		oss << '-' << crc24_to_text(compute_crc24(k, n, s.data));
	}
	return oss.str();
}

} // namespace

int main(int argc, char* argv[])
{
	using std::cout;
	using std::cerr;
	using std::endl;
	using std::exception;
	try {
		static const octet arr[] = {'H','e','l','l','o','\n'};
		auto shares = secret_share(arr, 2, 4);
		assert(shares.size() == 4);
		for (const auto& s : shares) {
			cout << share_to_text(2, s, true) << endl;
		}
		shares.erase(shares.begin());
		shares.erase(shares.begin()+1);
		auto secret = decode_secret(shares);
		for (unsigned k=0; k<secret.size(); ++k) {
			cout << ' ' << unsigned(secret[k]);
		}
		cout << endl;
	} catch (exception const& x) {
		cerr << "Exception: " << x.what() << endl;
		return EXIT_FAILURE;
	}
}

