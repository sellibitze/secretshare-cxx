#include <cstddef>
#include <exception>
#include <iostream>
#include <vector>
#include <string>
#include "slice.hpp"
#include "base64.hpp"

using std::cout;
using std::cerr;
using std::endl;

namespace {

void show_hex(slice<const unsigned char> data)
{
	static const char digits[] = "0123456789ABCDEF";
	while (data) {
		unsigned bite = data.shift();
		cout << digits[(bite >> 4) & 0xF];
		cout << digits[(bite     ) & 0xF];
	}
	cout << '\n';
}

bool check1()
{
	static const unsigned char data[] = {23,42,99,17,29,177,01,255};
	auto text = base64_enc(data);
	auto vec = base64_dec(text);
	show_hex(data);
	cout << text << endl;
	show_hex(data);
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

