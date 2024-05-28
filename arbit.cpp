#include "arbit.hpp"

namespace wc
{
	arbit::arbit(arbit&& other)
		:precision(other.precision),
		 fixed_ptr(other.fixed_ptr), decimal_ptr(other.decimal_ptr),
		 fixed_len(other.fixed_len), decimal_len(other.decimal_len)
	{
		other.fixed_ptr = other.decimal_ptr = nullptr;
		other.fixed_len = other.decimal_len = 0;
	}

	arbit::arbit(base_t fixed, base_t decimal, uint8_t precision)
		:precision(precision)
	{
		if (decimal != 0)
		{
			if (!(decimal_ptr = (base_t*)malloc(sizeof(base_t) * 1)))
				WC_STD_EXCEPTION("Failed to allocate memory for the decimals");
			decimal_len = 1;
			decimal_ptr[0] = decimal;
		}

		if (!(fixed_ptr = (base_t*)malloc(sizeof(base_t) * 1)))
			WC_STD_EXCEPTION("Failed to allocate memory for the fixeds");
		fixed_len = 1;
		fixed_ptr[0] = fixed;
	}

	arbit::~arbit()
	{
		if (fixed_ptr) free(fixed_ptr);
		if (decimal_ptr) free(decimal_ptr);
	}

	arbit& arbit::operator+=(const arbit& rhs)
	{
		return *this;
	}

	void arbit::raw_print()
	{
		std::print("Fixed: {}", fixed_len);
		if (fixed_len > 0)
		{
			std::print("> ");
			for (unsigned i=0; i < fixed_len; i++)
				std::print("{} ", fixed_ptr[i]);
		}
		if (decimal_len > 0)
		{
			std::print(", Decimal: {}> ", decimal_len);
			for (unsigned i=0; i < decimal_len; i++)
				std::print("{} ", decimal_ptr[i]);
		}
		std::println("");
	}
};
