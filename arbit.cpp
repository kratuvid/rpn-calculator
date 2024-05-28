#include "arbit.hpp"

namespace wc
{
	arbit::arbit(arbit&& other)
		:precision(other.precision), neg(other.neg),
		 fixed_len(other.fixed_len), decimal_len(other.decimal_len),
		 actual_fixed_len(other.actual_fixed_len), actual_decimal_len(other.actual_decimal_len)
	{
		if (fixed_ptr) free(fixed_ptr);
		if (decimal_ptr) free(decimal_ptr);

		fixed_ptr = other.fixed_ptr;
		decimal_ptr = other.decimal_ptr;

		other.fixed_ptr = other.decimal_ptr = nullptr;
		other.fixed_len = other.decimal_len = 0;
		other.actual_fixed_len = other.actual_decimal_len = 0;
		other.precision = default_precision;
	}

	arbit::arbit(std::string_view both, base_t precision)
		:precision(precision)
	{
		parse(both);
	}

	arbit::arbit(base_t fixed, base_t decimal, bool neg, base_t precision)
		:precision(precision)
	{
		if (decimal != 0)
		{
			if (!(decimal_ptr = (base_t*)malloc(sizeof(base_t) * 2)))
				WC_STD_EXCEPTION("Failed to allocate memory for the decimals");
			decimal_len = 1;
			actual_decimal_len = 2;
			decimal_ptr[0] = decimal;
		}

		if (!(fixed_ptr = (base_t*)malloc(sizeof(base_t) * 2)))
			WC_STD_EXCEPTION("Failed to allocate memory for the fixeds");
		fixed_len = 1;
		actual_fixed_len = 2;
		fixed_ptr[0] = fixed;
	}

	arbit::~arbit()
	{
		if (fixed_ptr) free(fixed_ptr);
		if (decimal_ptr) free(decimal_ptr);
	}

	bool arbit::negative()
	{
		if (fixed_len > 0)
		{
			if (fixed_ptr[fixed_len-1] >> ((sizeof(base_t) * 8) - 1))
				return true;
		}
		return false;
	}

	arbit& arbit::operator+=(const arbit& rhs)
	{
		return *this;
	}

	void arbit::raw_print()
	{
		std::print("Neg: {} , ", neg);
		std::print("Fixed: {},{}", actual_fixed_len, fixed_len);
		if (fixed_len > 0)
		{
			std::print("> ");
			for (unsigned i=0; i < fixed_len; i++)
				std::print("{} ", fixed_ptr[i]);
		}
		if (decimal_len > 0)
		{
			std::print(", Decimal: {},{}> ", actual_decimal_len, decimal_len);
			for (unsigned i=0; i < decimal_len; i++)
				std::print("{} ", decimal_ptr[i]);
		}
		std::println("");
	}

	/* private members */

	void arbit::parse(std::string_view both)
	{
		if (both.size() == 0)
			WC_ARBIT_EXCEPTION(parse, "Empty string");

		bool is_decimal = false;
		std::string_view::iterator fixed_end = both.end();

		auto it = both.begin();
		if (both[0] == '-')
		{
			neg = true;
			it = both.begin()+1;
		}

		for (; it != both.end(); it++)
		{
			char c = *it;
			if (c == '.')
			{
				if (is_decimal)
					WC_ARBIT_EXCEPTION(parse, "Encountered a second decimal point in '{}'", both);
				if (it != both.end()-1)
					is_decimal = true;
				fixed_end = it;
				continue;
			}
			else if (!isdigit(c))
			{
				WC_ARBIT_EXCEPTION(parse, "'{}' is not a digit in '{}'", c, both);
			}
		}

		std::string_view fixed(both.begin(), fixed_end), decimal;
		if (is_decimal)
			decimal = std::string_view(std::next(fixed_end), both.end());
		parse(fixed, decimal);
	}

	void arbit::parse(std::string_view fixed, std::string_view decimal)
	{
	}

	void arbit::grow(size_t by)
	{
		const bool neg = negative();

		const auto has_len = actual_fixed_len - fixed_len;
		if (by <= has_len)
		{
			memset(&fixed_ptr[fixed_len], neg ? 0xff : 0x00, sizeof(base_t) * by);
			fixed_len += by;
		}
		else
		{
			const size_t grow_const = 2, grow_upper_limit = 1000;
			const auto new_actual_fixed_len = actual_fixed_len + by + grow_const;
			const auto new_fixed_len = fixed_len + by;

			if (new_actual_fixed_len > grow_upper_limit)
				WC_STD_EXCEPTION("Cannot grow to {} as {} is the upper limit",
								 new_actual_fixed_len, grow_upper_limit);

			if (!(fixed_ptr = (base_t*)realloc(fixed_ptr, sizeof(base_t) * new_actual_fixed_len)))
				WC_STD_EXCEPTION("Failed to reallocate from length {} to {}",
								 actual_fixed_len, new_actual_fixed_len);

			memset(&fixed_ptr[actual_fixed_len], neg ? 0xff : 0x00, sizeof(base_t) * by);

			actual_fixed_len = new_actual_fixed_len;
			fixed_len = new_fixed_len;
		}
	}

	void arbit::shrink(size_t by)
	{
		if (by > fixed_len)
			WC_STD_EXCEPTION("Cannot shrink by {} when {} is all it has", by, fixed_len);

		const size_t new_fixed_len = fixed_len - by;

		/*
		if(!(fixed_ptr = (base_t*)realloc((void*)fixed_ptr, sizeof(base_t) * new_fixed_len)))
			WC_STD_EXCEPTION("Failed to reallocate from length {} to {}", fixed_len, new_fixed_len);
		*/

		fixed_len = new_fixed_len;
	}
};
