#include <print>
#include <cstdint>
#include <iostream>
using namespace std;

using use_t = uint8_t;
using suse_t = int8_t;
using use_double_t = uint16_t;
using use_quad_t = uint32_t;

int get_int(use_t n)
{
	unsigned begin = (n >> 3) & 0x1 ? ~unsigned(0) : 0;
	begin >>= 4; begin <<= 4;
	begin |= n;
	return begin;
}

int get_int2(use_t n)
{
	unsigned begin = (n >> 7) & 0x1 ? ~unsigned(0) : 0;
	begin >>= 8; begin <<= 8;
	begin |= n;
	return begin;
}

use_t mul(use_t a, use_t b)
{
	use_t p = 0;

	size_t i=0;
	while (b >> i)
	{
		use_t bit = (b >> i) & 0x1;
		use_t shifted_a = a << i;

		if (bit)
			p += shifted_a;

		i++;
	}

	return p;
}

int main()
{
	/*
	use_t a, b; int aa, bb;
	cout << "A? "; cin >> aa;
	cout << "B? "; cin >> bb;
	a = aa; a &= 0x0f;
	b = bb; b &= 0x0f;
	*/

	for (int aa = -8; aa <= 7; aa++)
	{
		for (int bb = -8; bb <= 7; bb++)
		{
			use_t a = aa, b = bb;

			print("{:0>8b} ({}) * ", a, get_int(a));
			print("{:0>8b} ({}) = ", b, get_int(b));

			use_t p = mul(a, b);
			println("{:0>8b} ({})", p, get_int2(p));

			if ((aa * bb) != get_int2(p))
				throw false;
		}
	}
}
