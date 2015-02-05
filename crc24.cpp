#include "crc24.hpp"

namespace {

const std::uint32_t REDUCE_POLY = 0x1864CFB;
const std::uint32_t INIT_VALUE  =  0xB704CE;

struct table {
	std::uint32_t arr[256];
	table();
};

table::table()
{
	for (std::uint32_t bite=0; bite<256; ++bite) {
		auto temp = bite << 16;
		for (int k=0; k<8; ++k) {
			temp = (temp << 1) ^ ((-(temp >> 23)) & REDUCE_POLY);
		}
		arr[bite] = temp;
	}
}

table const& get_table()
{
	static const table t;
	return t;
}

inline std::uint32_t process_byte_impl(std::uint32_t state,
                                       unsigned char bite,
                                       table const& t)
{
	return (state << 8) ^ t.arr[((state >> 16) ^ bite) & 0xFF];
}

} // namespace

void crc24hasher::process_byte(unsigned char bite)
{
	auto& tab = get_table();
	state = process_byte_impl(state, bite, tab) & 0xFFFFFF;
}

void crc24hasher::process_block(slice<const unsigned char> data)
{
	if (!data) return;
	auto& tab = get_table();
	auto temp = this->state;
	do {
		unsigned bite = data.shift();
		temp = process_byte_impl(temp, bite, tab);
	} while (data);
	this->state = temp & 0xFFFFFF;
}

