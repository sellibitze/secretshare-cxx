#ifndef INC_GUARD_GBJSJ1K0UDF9JNQQUKHXNVXY4DOUJH
#define INC_GUARD_GBJSJ1K0UDF9JNQQUKHXNVXY4DOUJH

#include <cassert>
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <vector>

#define REQUIRES(...) \
	,class=typename std::enable_if<(__VA_ARGS__)>::type

#define VALID_ARRAY_PTR_CONVERSION(X, Y) \
	( std::is_convertible<X*, Y*>::value && (sizeof(X) == sizeof(Y)) )

template<class T>
class slice
{
public:
	template<class U
		REQUIRES( VALID_ARRAY_PTR_CONVERSION(U, T) )
	>
	explicit slice(U* base, std::size_t len) : base(base), len(len) {}

	template<class U, std::size_t N
		REQUIRES( VALID_ARRAY_PTR_CONVERSION(U, T) )
	>
	slice(U (&arr)[N]) : base(arr), len(N) {}

	template<class U, class A
		REQUIRES( VALID_ARRAY_PTR_CONVERSION(const U, T) )
	>
	slice(std::vector<U,A> const& x) : base(x.data()), len(x.size()) {}

	template<class U, class A
		REQUIRES( VALID_ARRAY_PTR_CONVERSION(U, T) )
	>
	slice(std::vector<U,A> & x) : base(x.data()), len(x.size()) {}

	template<class U
		REQUIRES( VALID_ARRAY_PTR_CONVERSION(U, T) )
	>
	slice(slice<U> x) : base(x.base), len(x.len) {}

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

	T& shift()
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

	slice slice_from(std::size_t from) const
	{
		assert(from <= len);
		return slice(base + from, len - from);
	}

private:
	T* base;
	std::size_t len;
};

#undef REQUIRES
#undef VALID_ARRAY_PTR_CONVERSION

template<class T, class A>
inline slice<const T> all_of(std::vector<T,A> const& x)
{ return slice<const T>(x); }

template<class T, class A>
inline slice<T> all_of(std::vector<T,A> & x)
{ return slice<T>(x); }

template<class T, std::size_t N>
inline slice<T> all_of(T (&x)[N])
{ return slice<T>(x); }

template<class T, class A>
inline slice<const T> const_all_of(std::vector<T,A> const& x)
{ return slice<const T>(x); }

template<class T, std::size_t N>
inline slice<const T> const_all_of(T const (&x)[N])
{ return slice<const T>(x); }

#endif /* include guard */

