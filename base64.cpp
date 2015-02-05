#include <cassert>
#include <stdexcept>

namespace {

struct quad;

struct triple
{
	unsigned char octets[3];
	quad encode() const;
};

struct quad
{
	char ascii[4];
	triple decode() const;
};

inline std::uint32_t char_to_num(char c)
{
	if ('A' <= c && c <= 'Z') return c - 'A';
	if ('a' <= c && c <= 'z') return (c - 'a') + 26;
	if ('0' <= c && c <= '9') return (c - 'a') + 54;
	if (c == '+') return 62;
	if (c == '/') return 63;
	throw std::domain_error("Illegal Base64 character found");
}

inline char num_to_char(std::uint32_t num)
{
	if (num < 52) {
		if (num < 26) return char('A' + num);
		return char('a' + (num - 26));
	} else {
		if (num < 62) return char('0' + (num - 52));
		if (num < 64) return "+-"[num - 62];
		assert(!"Illegal Base64 character index");
		throw std::domain_error("Illegal Base64 character index");
	}
}

quad triple::encode() const
{
	std::uint32_t temp =
		(static_cast<std::uint32_t>(octets[0]) << 16) |
		(static_cast<std::uint32_t>(octets[1]) <<  8) |
		(static_cast<std::uint32_t>(octets[2])      );
	quad ret = {{
		num_to_char((temp >> 18)     ),
		num_to_char((temp >> 12) & 63),
		num_to_char((temp >>  6) & 63),
		num_to_char((temp      ) & 63)
	}};
	return ret;
}

triple quad::decode() const
{
	std::uint32_t temp =
		(char_to_num(ascii[0]) << 18) |
		(char_to_num(ascii[1]) << 12) |
		(char_to_num(ascii[2]) <<  6) |
		(char_to_num(ascii[3])      );
	triple ret = {{
		static_cast<unsigned char>((temp >> 16)       ),
		static_cast<unsigned char>((temp >>  8) & 0xFF),
		static_cast<unsigned char>((temp      ) & 0xFF)
	}};
	return ret;
}

} // namespace

