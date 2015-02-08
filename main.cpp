#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "base64.hpp"
#include "crc24.hpp"
#include "getrandom.hpp"
#include "io.hpp"
#include "poly.hpp"
#include "slice.hpp"

using std::atoi;
using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::exception;
using std::istream;
using std::istringstream;
using std::make_pair;
using std::move;
using std::ostream;
using std::ostringstream;
using std::pair;
using std::runtime_error;
using std::size_t;
using std::strcmp;
using std::streamsize;
using std::string;
using std::uint32_t;
using std::vector;

namespace {

struct share
{
	octet xcoord;
	vector<octet> data;
	explicit share(size_t bs) : xcoord(0), data(bs) {}
	share(octet x, vector<octet> d) : xcoord(x), data(move(d)) {}
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

pair<unsigned,share> text_to_share(string const& t)
{
	istringstream in (t);
	int k, n;
	char m1, m2;
	if (in >> k >> m1 >> n >> m2 && m1=='-' && m2=='-') {
		if (k<0 || n<0 || k>255 || n>255) {
			throw runtime_error("Illegal K,N values in share");
		}
		string remainder;
		in >> remainder;
		auto pos = remainder.find('-');
		auto dat = base64_dec(remainder.substr(0, pos));
		if (pos != string::npos) {
			pos += 1; // ignore -
			if (pos+4 > remainder.size()) {
				throw runtime_error("encoded CRC-24 must be four characters long");
			}
			auto crc_found = remainder.substr(pos, pos+4);
			auto crc_computed = crc24_to_text(compute_crc24(k, n, dat));
			if (crc_found != crc_computed) {
				throw runtime_error("CRC-24 mismatch");
			}
		}
		share s = { octet(n), move(dat) };
		return make_pair(unsigned(k), move(s));
	} else {
		throw runtime_error("Parsing share error");
	}
}

enum class mode {
	dontknow,
	encode,
	decode
};

vector<octet> read_secret_from(istream& in)
{
	vector<octet> result;
	octet temp;
	char& alias = reinterpret_cast<char&>(temp);
	// ^ is OK according to aliasing rules because temp and alias
	// are both referring to something of a character type.
	while (in.get(alias)) {
		if (result.size() == 0x10000U) {
			throw runtime_error("Secret too large. "
			                    "Do you really need more then 64KB?!");
		}
		result.push_back(temp);
	}
	return result;
}

vector<share> read_shares_from(istream& in)
{
	unsigned k = 0;
	vector<share> result;
	string word;
	while (cin >> word) {
		auto p = text_to_share(word);
		if (result.empty()) {
			// simply copy K value for the first share
			k = p.first;
		} else {
			// enforce consistency of K value and size for all shares
			if (k != p.first ||
			    result.front().data.size() != p.second.data.size())
			{
				throw runtime_error("Incompatible shares");
			}
		}
		// check for duplicates
		bool duplicate = false;
		for (const auto& s : result) {
			if (s.xcoord == p.second.xcoord) {
				if (s.data != p.second.data) {
					throw runtime_error("Inconsistent share data");
				}
				duplicate = true;
			}
		}
		if (!duplicate) {
			result.push_back(move(p.second));
			if (result.size() == k) {
				return result;
			}
		}
	}
	throw runtime_error("Not enough shares provided");
}

void do_encode(unsigned k, unsigned n)
{
	binary_mode_for_stdin();
	auto v = read_secret_from(cin);
	auto shares = secret_share(v, k, n);
	for (const auto& s : shares) {
		cout << share_to_text(k, s, true) << endl;
	}
}

void do_decode()
{
	auto shares = read_shares_from(cin);
	auto secret = decode_secret(shares);
	binary_mode_for_stdout();
	cout.write(reinterpret_cast<const char*>(secret.data()), secret.size());
}

} // namespace

int main(int argc, char* argv[])
{
	try {
		int k = 0;
		int n = 0;
		mode m = mode::dontknow;
		if (argc >= 2) {
			if (strcmp(argv[1], "e") == 0) m = mode::encode; else
			if (strcmp(argv[1], "d") == 0) m = mode::decode;
		}
		if (m == mode::encode) {
			if (argc < 4) {
				cerr << "Missing two encoding parameters\n";
				return EXIT_FAILURE;
			}
			k = atoi(argv[2]);
			n = atoi(argv[3]);
			if (k < 1 || k > 255) {
				cerr << "K needs to be within 1...255\n";
				return EXIT_FAILURE;
			}
			if (n < k || n > 255) {
				cerr << "N needs to be within K...255\n";
				return EXIT_FAILURE;
			}
		}
		switch (m) {
			case mode::encode:
				do_encode(k, n);
				break;
			case mode::decode:
				do_decode();
				break;
			case mode::dontknow:
				cout << "\n"
"This program sss is an implementation of Shamir's secret sharing scheme.\n"
"\n"
"Usage: sss <mode> [K N]\n"
"\n"
"Encoding (reading secret data from STDIN and writing N shares to STDOUT of\n"
"which K shares are necessary to decode the secret again):\n"
"   secretshare e K N\n"
"\n"
"Decoding (reading shares from STDIN and writing decoded secret to STDOUT)\n"
"   secretshare d\n" << endl;
				return argc==1 ? EXIT_SUCCESS : EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	} catch (exception const& x) {
		cerr << "Exception: " << x.what() << endl;
		return EXIT_FAILURE;
	}
}

