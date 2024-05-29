#include "arbit.hpp"

namespace wc
{
	arbit::arbit(const arbit& other)
		:precision(other.precision),
		 fixed_len(other.fixed_len), decimal_len(other.decimal_len),
		 actual_fixed_len(other.fixed_len), actual_decimal_len(other.decimal_len)
	{
		if (fixed_ptr) free(fixed_ptr);
		if (decimal_ptr) free(decimal_ptr);

		fixed_ptr = (base_t*)malloc(actual_fixed_len);
		decimal_ptr = (base_t*)malloc(actual_decimal_len);

		memcpy(fixed_ptr, other.fixed_ptr, sizeof(base_t) * fixed_len);
		memcpy(decimal_ptr, other.decimal_ptr, sizeof(base_t) * decimal_len);
	}

	arbit::arbit(arbit&& other)
		:precision(other.precision),
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

	arbit::arbit(base_t fixed, base_t decimal, base_t precision)
		:precision(precision)
	{
		if (decimal != 0)
		{
			WC_STD_EXCEPTION("{}:{}: Decimal unimplemented", __FILE__, __LINE__);
		}

		grow(1, is_base_t_negative(fixed));
		fixed_ptr[0] = fixed;
	}

	arbit::arbit(std::initializer_list<base_t> fixed, std::initializer_list<base_t> decimal, base_t precision)
		:precision(precision)
	{
		if (fixed.size() > 0)
		{
			const bool neg = is_base_t_negative(*(fixed.begin() + fixed.size() - 1));
			grow(fixed.size(), neg);

			size_t i=0;
			auto it = fixed.begin();
			for (; it != fixed.end(); it++, i++)
				fixed_ptr[i] = *it;
		}
		else
			grow(1);

		if (decimal.size() > 0)
		{
			WC_STD_EXCEPTION("{}:{}: Decimal unimplemented", __FILE__, __LINE__);
		}
	}

	arbit::~arbit()
	{
		if (fixed_ptr) free(fixed_ptr);
		if (decimal_ptr) free(decimal_ptr);
	}

	bool arbit::is_negative() const
	{
		if (fixed_len > 0)
		{
			if (fixed_ptr[fixed_len-1] >> ((sizeof(base_t) * 8) - 1))
				return true;
		}
		return false;
	}

	arbit& arbit::negate()
	{
		for (size_t i=0; i < fixed_len; i++)
			fixed_ptr[i] = ~fixed_ptr[i];
		*this += 1;
		return *this;
	}

	arbit arbit::operator-() const
	{
		arbit copy(*this);
		copy.negate();
		return copy;
	}

	void arbit::raw_print(bool hex) const
	{
		std::print("Fixed: {},{}", actual_fixed_len, fixed_len);
		if (fixed_len > 0)
		{
			std::print("> ");
			for (unsigned i=0; i < fixed_len; i++)
			{
				auto unit = fixed_ptr[i];
				if (hex)
					std::print("{:#x} ", fixed_ptr[i]);
				else
				{
					std::print("{}", fixed_ptr[i]);
					if (is_base_t_negative(unit))
						std::print("|{}", sbase_t(unit));
					std::print(" ");
				}
			}
		}
		if (decimal_len > 0)
		{
			std::print(", Decimal: {},{}> ", actual_decimal_len, decimal_len);
			for (unsigned i=0; i < decimal_len; i++)
				if (hex)
					std::print("{:#x} ", decimal_ptr[i]);
				else
					std::print("{} ", decimal_ptr[i]);
		}
		std::println("");
	}

	void arbit::print()
	{
		std::string digits;

		const bool neg = is_negative();

		if (neg) negate();

		for (size_t i=0; i < fixed_len; i++)
		{
			base_t n = fixed_ptr[i];
			while (n != 0)
			{
				digits += std::to_string(n % 10);
				n /= 10;
			}
		}
		if (digits.empty()) digits += "0";
		std::reverse(digits.begin(), digits.end());

		if (neg) negate();

		if (neg) std::print("-");
		std::print("{}", digits);

		if (decimal_len > 0)
			std::print(".IMPL");
	}

	/* private members */

	void arbit::parse(std::string_view both)
	{
		if (both.size() == 0)
			WC_ARBIT_EXCEPTION(parse, "Empty string");

		bool is_decimal = false;
		std::string_view::iterator fixed_end = both.end();

		bool neg = false;
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
		parse(fixed, decimal, neg);
	}

	void arbit::parse(std::string_view fixed, std::string_view decimal, bool neg)
	{
		WC_STD_EXCEPTION("{}:{}: parse is broken", __FILE__, __LINE__);

		for (auto it = fixed.rbegin(); it != fixed.rend(); it++)
		{
		}
	}

	void arbit::grow(size_t by)
	{
		grow(by, is_negative());
	}

	void arbit::grow(size_t by, bool neg)
	{
		const auto has_len = actual_fixed_len - fixed_len;
		if (by <= has_len)
		{
			memset(&fixed_ptr[fixed_len], neg ? 0xff : 0x00, sizeof(base_t) * by);
			fixed_len += by;
		}
		else
		{
			const size_t grow_const = 1, grow_upper_limit = 1000;
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
