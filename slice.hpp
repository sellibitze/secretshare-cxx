#ifndef INC_GUARD_GBJSJ1K0UDF9JNQQUKHXNVXY4DOUJH
#define INC_GUARD_GBJSJ1K0UDF9JNQQUKHXNVXY4DOUJH

#include <cassert>
#include <cstddef>
#include <stdexcept>

template<class T>
class slice
{
public:
	explicit slice(T* base, std::size_t len) : base(base), len(len) {}

	std::size_t size() const { return len; }
	bool empty() const { return len==0; }
	explicit operator bool() const { return len!=0; }
	T* begin() const { return base; }
	T* end() const { return base + len; }

	T& operator[](std::size_t index) const
	{
		assert(index < len);
		return base[index];
	}

	&T shift()
	{
		if (empty()) throw std::logic_error("slice already empty");
		auto old = base;
		++base;
		--len;
		return *old;
	}

	T& pop()
	{
		if (empty()) throw std::logic_error("slice already empty");
		--len;
		return base[len];
	}

private:
	T* base;
	std::size_t len;
};

#endif /* include guard */

