#include <print>
#include <format>
#include <cstdint>
#include <cmath>
#include <sstream>
using namespace std;

using base_t = uint64_t;
using base_half_t = uint32_t;

std::string get_raw_repr(base_t n, bool hex = true)
{
	if (hex)
		return format("I:0x{:0>8x} F:0x{:0>8x}", n >> 32, n & ~base_half_t(0));
	else
		return format("I:{} F:{}", n >> 32, n & ~base_half_t(0));
}

std::string get_repr(base_t n)
{
	const bool neg = n >> 63;
	if (neg)
		n = ~n + 1;

	const base_half_t integer = n >> 32, fraction = n & ~base_half_t(0);

	long double fraction_ld = 0;
	base_t powers = 2;
	for (int i = sizeof(base_half_t) * 8 - 1; i >= 0; i--)
	{
		if ((fraction >> i) & 0x1)
			fraction_ld += 1.0L / powers;
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

	std::ostringstream oss;
	if (neg) oss << '-';
	oss << integer << "." << fstr;

	return oss.str();
}

base_t make_fp(base_half_t integer, base_half_t fraction, bool neg)
{
	base_t n = (base_t(integer) << 32) | base_t(fraction);
	if (neg)
		n = ~n + 1;
	return n;
}

std::string get_both_repr(base_t n)
{
	return format("raw: {}, readable: {}", get_raw_repr(n), get_repr(n));
}

int main()
{
	const auto n1 = make_fp(0, 0b11 << 30, false);
	const auto n2 = make_fp(0, 0b01 << 30, false);
	const auto s = n1 + n2;
	const auto d = n1 - n2;

	println("n1: {}", get_both_repr(n1));
	println("n2: {}", get_both_repr(n2));
	println("s: {}", get_both_repr(s));
	println("d: {}", get_both_repr(d));
}
