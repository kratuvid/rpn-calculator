#include <print>
#include <format>
#include <cstdint>
#include <cmath>
#include <random>
using namespace std;

using base_double_t = uint64_t;
using base_t = uint32_t;
using base_half_t = uint16_t;

template<class F>
std::string get_raw_repr(F n, bool hex = true)
{
	const auto bits = sizeof(F) * 8, bits_half = bits / 2;
	if (hex)
		return format("I:0x{:0>8x} F:0x{:0>8x}",
					  n >> bits_half, (n << bits_half) >> bits_half);
	else
		return format("I:{} F:{}", n >> bits_half, (n << bits_half) >> bits_half);
}

template<class F>
std::string get_repr(F n)
{
	const auto bits = sizeof(F) * 8, bits_half = bits / 2;
	const bool neg = n >> (bits - 1);
	if (neg)
		n = ~n + 1;

	const F integer = n >> bits_half, fraction = (n << bits_half) >> bits_half;

	long double fraction_ld = 0;
	base_t powers = 2;
	for (int i = bits_half-1; i >= 0; i--)
	{
		if ((fraction >> i) & 0x1)
		{
			const auto c = 1.0L / powers;
			if (!isinf(c))
				fraction_ld += c;
		}
		powers *= 2;
	}

	std::string fstr = std::to_string(fraction_ld);
	int i=0;
	for (; i < (int)fstr.size(); i++)
		if (fstr[i] == '.')
		{
			i++;
			break;
		}
	fstr = fstr.substr(i);

	return format("{}{}.{}", neg ? "-" : "", integer, fstr);
}

template<class F>
F make_fp(F integer, F fraction, bool neg)
{
	const auto bits = sizeof(F) * 8, bits_half = bits / 2;
	F n = (integer << bits_half) | fraction;
	if (neg)
		n = ~n + 1;
	return n;
}

template<class F>
std::string get_both_repr(F n)
{
	return format("raw: {}, readable: {}", get_raw_repr(n), get_repr(n));
}

base_double_t mul(base_t aa, base_t bb)
{
	base_double_t product = 0;
	const base_double_t a = aa >> 31 ? (~base_double_t(0) << 32 | aa) : aa;
	const base_double_t b = bb >> 31 ? (~base_double_t(0) << 32 | bb) : bb;

	size_t i = 0;
	while (i < sizeof(base_double_t)*8)
	{
		auto bit = (b >> i) & 0x1;
		auto shifted_a = a << i;

		if (bit)
			product += shifted_a;

		i++;
	}

	return product;
}

int main()
{
	random_device rd;
	mt19937 re(rd());
	uniform_int_distribution<base_half_t> dist(0, ~base_half_t(0)), dist2(0, 20), dist3(0, 1);

	const auto n1 = make_fp<base_t>(dist2(re), dist(re), dist3(re));
	const auto n2 = make_fp<base_t>(dist2(re), dist(re), dist3(re));
	const auto s = n1 + n2;
	const auto d = n1 - n2;
	const auto p = mul(n1, n2);

	println("n1: {}", get_both_repr(n1));
	println("n2: {}", get_both_repr(n2));
	println("s: {}", get_both_repr(s));
	println("d: {}", get_both_repr(d));
	println("p: {}", get_both_repr(p));
}
