#include <cstddef>
#include <iostream>
#include <exception>
#include "gf256.hpp"

using std::cout;
using std::cerr;
using std::endl;

namespace {

bool check1()
{
	for (unsigned x=1; x<256; ++x) {
		auto t = gf256(x);
		auto l = log(t);
		auto p = gf256::exp(l);
		if (p != t) return false;
	}
	return true;
}

} // namespace

int main()
{
	try {
		bool ok = true;
		ok = ok && check1();
		return ok ? EXIT_SUCCESS : EXIT_FAILURE;
	} catch (std::exception const& x) {
		cerr << "Exception: " << x.what() << endl;
		return EXIT_FAILURE;
	}
}

