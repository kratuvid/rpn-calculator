import <print>;
import <cstdint>;
import <string>;
using namespace std;

using base_t = int32_t;

auto cdiv(base_t num, base_t den)
{
	const bool final_neg = (num < 0) ^ (den < 0);
	num = num < 0 ? -num : num;
	den = den < 0 ? -den : den;

	base_t quotient = 0, remainder = num;

	while (num >= den)
	{
		num -= den;
		quotient++;
		remainder = num;
	}

	if (final_neg) quotient = -quotient;
	struct { base_t quotient, remainder; } r {quotient, remainder};
	return r;
}

void do_cdiv(base_t num, base_t den)
{
	const auto result = cdiv(num, den);
	println("{} / {} = {}, {}", num, den, result.quotient, result.remainder);
}

int main(int argc, char** argv)
{
	int act = 1;
	if (argc > 1)
		act = stoi(argv[1]);

	switch (act)
	{
	case 0:
	{
		do_cdiv(238, 49);
	} break;

	case 1:
	{
		for (int n = 0; n < 100; n++)
		{
			for (int d = 1; d <= n; d++)
			{
				do_cdiv(n, d);
			}
		}
	} break;
	}
}
