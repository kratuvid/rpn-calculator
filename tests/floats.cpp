#include <print>
#include <iostream>
#include <cstdint>
#include <cmath>
using namespace std;

std::string binary(uint32_t i, int starting = 0, bool comma = true)
{
	std::string str;
	bool one = false;
	for (int j = 31; j >= 0; j--)
	{
		const auto bit = (i >> j) & 0x1;
		if (!one && bit) one = true;
		if (!str.empty() && (j == 30 || j == 22) && comma) str += ",";
		if (one || (j <= starting)) str += bit + '0';
	}
	if (str.empty()) str += '0';
	return str;
}

void extract(float f, uint32_t& sign, uint32_t& exp, int32_t& exp_norm, uint32_t& mantissa)
{
	const uint32_t* ff = (uint32_t*)&f;
	sign = *ff >> 31;
	exp = (*ff >> 23) & ~(1 << 8);
	exp_norm = exp - 127;
	mantissa = *ff & ~(0x1ff << 23);
}

int main()
{
	cout << "Henceforth, enter floats..." << endl;

	bool quit = false;
	while (!quit)
	{
		std::string line;
		cout << "> "; cout.flush();
		if (!getline(cin, line) || line == "quit" || line == "exit" || line == "q" || line == "e")
		{
			quit = true;
			continue;
		}

		char* str_end = nullptr;
		float f = strtof(line.c_str(), &str_end);
		if (str_end == line.c_str())
		{
			cout << "Floating point conversion failed..." << endl;
			continue;
		}
		
		const uint32_t* ff = (uint32_t*)&f;

		uint32_t sign, exp, mantissa;
		int32_t exp_norm;
		extract(f, sign, exp, exp_norm, mantissa);

		auto category = fpclassify(f);
		std::string category_str;
		switch(category)
		{
		case FP_INFINITE: category_str = "infinite";
			break;
		case FP_NAN: category_str = "NaN";
			break;
		case FP_NORMAL: category_str = "normal";
			break;
		case FP_SUBNORMAL: category_str = "subnormal";
			break;
		case FP_ZERO: category_str = "zero";
			break;
		default: category_str = "unknown";
			break;
		}

		cout << f << " is " << binary(*ff, 31) << endl;
		cout << "Sign: " << boolalpha << (bool)sign << endl;
		cout << "Exponent: " << exp << ", 0x" << hex << exp << dec << ", " << binary(exp, 7) << endl;
		cout << "Exponent normalised: " << exp_norm << ", 0x" << hex << exp_norm << dec << ", " << binary(exp_norm, 7, false) << endl;
		cout << "Mantissa: " << mantissa << ", 0x" << hex << mantissa << dec << ", " << binary(mantissa, 22) << endl;
		cout << "Category: " << category_str << endl;
	}
}
