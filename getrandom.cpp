#include <fstream>
#include <stdexcept>
#include "getrandom.hpp"

using std::ifstream;
using std::ios_base;
using std::runtime_error;

#define DEVICE "/dev/urandom"

namespace {

ifstream& get_urandom()
{
	static ifstream urandom (DEVICE, ios_base::in | ios_base::binary);
	return urandom;
}

} // namespace

void get_random(slice<unsigned char> fill)
{
	ifstream& is = get_urandom();
	if (!is) throw runtime_error("Could not open " DEVICE);
	if (!is.read(reinterpret_cast<char*>(fill.begin()), fill.size()))
		throw runtime_error("Could not read from " DEVICE);
}

