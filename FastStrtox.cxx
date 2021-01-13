#include <cmath>

#include <array>
#include <concepts>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>


using si  =   signed       int;
using ui  = unsigned       int;
using sl  =   signed      long;
using ul  = unsigned      long;
using sll =   signed long long;
using ull = unsigned long long;


// FAST: no locale shit; only look for a subset of standard isspace chars
static bool fast_isspace(char c)
{
	switch (c) {
	case '\t':
	case '\n':
	case ' ':
		return true;
	default:
		return false;
	}
}

// FAST: no locale shit
static bool fast_isdigit(char c)
{
	return (c >= '0' && c <= '9');
}


template<typename T>
static constexpr size_t max_digits = std::ceil(std::numeric_limits<T>::digits * std::log10(2));


template<std::integral T, T... IDX>
static constexpr auto generate_pow10_array_internal(std::integer_sequence<T, IDX...>)
{
	constexpr size_t MAX = sizeof...(IDX) - 1;
	return std::array<T, sizeof...(IDX)>{ static_cast<T>(std::pow(10, MAX - IDX))... };
}

template<typename T>
static constexpr auto generate_pow10_array() { return generate_pow10_array_internal(std::make_integer_sequence<T, max_digits<T>>{}); }


template<std::integral T>
static inline T fast_strtox(char **str)
{
	const char *ptr = *str;
	T num = 0;

	// skip leading whitespace (if any)
	while (fast_isspace(*ptr)) {
		++ptr;
	}

	// early out
	if (*ptr == '\0') {
		*str = const_cast<char *>(ptr);
		return 0;
	}

	// handle sign characters (ignoring '+' in this case)
	bool negative = false;
	if constexpr (std::is_signed_v<T>) {
		if (*ptr == '-') {
			negative = true;
			++ptr;
		}
	}

	// precompute the number of digits
	size_t n_digits = 0;
	const char *end = ptr;
	while (fast_isdigit(*end)) {
		if (++n_digits > max_digits<T>) {
			throw std::runtime_error("too many digits!");
		}
		++end;
	}

	// if we have no digits, just give up
	if (n_digits == 0) {
		*str = const_cast<char *>(ptr);
		return 0;
	}

	// precomputed array with powers of 10: from 10^max_digits down to 10^0
	static constexpr auto POW10 = generate_pow10_array<T>();

	// this CAN potentially overflow, if the actual digits are such that the number exceeds its limits
	size_t pwr = max_digits<T> - (end - ptr);
	do {
		num += (POW10[pwr++] * (*ptr - '0'));
	} while (++ptr != end);

	*str = const_cast<char *>(ptr);
	if constexpr (std::is_signed_v<T>) {
		return (negative ? -num : num);
	} else {
		return num;
	}
}


extern "C"
{
	si  fast_strtoi  (char **str) { return fast_strtox<si >(str); }
	ui  fast_strtoui (char **str) { return fast_strtox<ui >(str); }
	sl  fast_strtol  (char **str) { return fast_strtox<sl >(str); }
	ul  fast_strtoul (char **str) { return fast_strtox<ul >(str); }
	sll fast_strtoll (char **str) { return fast_strtox<sll>(str); }
	ull fast_strtoull(char **str) { return fast_strtox<ull>(str); }
}
