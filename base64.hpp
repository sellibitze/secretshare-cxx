#ifndef INC_GUARD_O6KDMMAKJV35ZO6L6MTHTP0GS9JXQB
#define INC_GUARD_O6KDMMAKJV35ZO6L6MTHTP0GS9JXQB

#include <string>
#include <vector>
#include "slice.hpp"

std::string base64_enc(slice<const unsigned char> data);

std::vector<unsigned char> base64_dec(std::string const& text);

#endif /* include guard */

