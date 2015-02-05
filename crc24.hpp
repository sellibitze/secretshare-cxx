#ifndef INC_GUARD_NUOGFJG2LOWH78E1C58BJHNYX9TOSM
#define INC_GUARD_NUOGFJG2LOWH78E1C58BJHNYX9TOSM

#include <cstdint>
#include "slice.hpp"

class crc24hasher
{
public:
	crc24hasher();

	void process_block(slice<const unsigned char> data);
	void process_byte(unsigned char bite);

	std::uint32_t finish() const;

private:
	std::uint32_t state;
};

#endif /* include guard */

