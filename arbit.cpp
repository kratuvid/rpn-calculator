#include "arbit.hpp"

namespace wc
{
	arbit::arbit(const arbit& other)
	{
		*this = other;
	}

	arbit::arbit(arbit&& other)
		:precision(other.precision),
		 fixed_len(other.fixed_len), decimal_len(other.decimal_len),
		 actual_fixed_len(other.actual_fixed_len), actual_decimal_len(other.actual_decimal_len)
	{
		if (fixed_ptr) {
			free(fixed_ptr);
			fixed_ptr = nullptr;
		}
		if (decimal_ptr) {
			free(decimal_ptr);
			decimal_ptr = nullptr;
		}

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
	{
		const auto f = {fixed};
		// const auto d = {decimal};
		arbit(f, {}, precision);
	}

	arbit::~arbit()
	{
		if (fixed_ptr) free(fixed_ptr);
		if (decimal_ptr) free(decimal_ptr);
	}

	void arbit::reset()
	{
		if (fixed_ptr) {
			free(fixed_ptr);
			fixed_ptr = nullptr;
		}
		if (decimal_ptr) {
			free(decimal_ptr);
			decimal_ptr = nullptr;
		}

		precision = default_precision;
		fixed_len = decimal_len = 0;
		actual_fixed_len = actual_decimal_len = 0;
	}

	bool arbit::bit(size_t at) const
	{
		const size_t unit = at / base_bits, unit_at = at % base_bits;
		if (unit <= fixed_len-1)
			return fixed_ptr[unit] & (1 << unit_at);
		return false;
	}

	void arbit::clear_bit(size_t at)
	{
		const size_t unit = at / base_bits, unit_at = at % base_bits;
		if (unit <= fixed_len-1)
			fixed_ptr[unit] &= ~(1 << unit_at);
	}

	void arbit::set_bit(size_t at)
	{
		const size_t unit = at / base_bits, unit_at = at % base_bits;
		if (unit <= fixed_len-1)
			fixed_ptr[unit] |= (1 << unit_at);
	}

	void arbit::flip_bit(size_t at)
	{
		const size_t unit = at / base_bits, unit_at = at % base_bits;
		if (unit <= fixed_len-1)
			fixed_ptr[unit] ^= (1 << unit_at);
	}

	void arbit::clear_first_bits(size_t before)
	{
		const size_t unit = before / base_bits, unit_at = before % base_bits;
		for (ssize_t i = unit; i >= 0; i--)
		{
			if ((size_t)i == unit)
			{
				fixed_ptr[i] >>= unit_at;
				fixed_ptr[i] <<= unit_at;
			}
			else
				fixed_ptr[i] = 0;
		}
	}

	bool arbit::is_zero() const
	{
		for (size_t i=0; i < fixed_len; i++)
			if (fixed_ptr[i] != 0)
				return false;
		return true;
	}

	bool arbit::is_negative() const
	{
		if (fixed_len > 0)
			return fixed_ptr[fixed_len-1] >> (base_bits - 1);
		return false;
	}

	bool arbit::is_negative(base_t n)
	{
		return n >> (base_bits - 1);
	}

	size_t arbit::bytes() const
	{
		return fixed_len * sizeof(base_t);
	}

	arbit& arbit::negate()
	{
		for (size_t i=0; i < fixed_len; i++)
			fixed_ptr[i] = ~fixed_ptr[i];
		*this += 1;
		return *this;
	}

	arbit& arbit::operator+=(const arbit& rhs)
	{
		if (rhs.fixed_len == 0)
			return *this;

		if (fixed_len < rhs.fixed_len)
		{
			const size_t by = rhs.fixed_len - fixed_len;
			grow(by);
		}

		return *this;
	}

	arbit arbit::operator*(const arbit& rhs)
	{
		arbit product(0), copy_rhs(rhs);

		while (!copy_rhs.is_zero())
		{
			product += *this;
			copy_rhs += -1;
		}

		return product;
	}

	arbit& arbit::operator<<=(size_t by)
	{
		const auto bits = bytes() * 8;
		if (by > bits)
		{
			memset(fixed_ptr, 0, bytes());
		}
		else if (by > 0)
		{
			for (ssize_t i = (ssize_t)bits-1; i >= 0; i--)
			{
				if ((size_t)i >= by)
				{
					const size_t at = (size_t)i - by;
					if (bit(at)) set_bit(i);
					else clear_bit(i);
				}
				else break;
			}
			const auto upto = by;
			clear_first_bits(upto);
		}

		return *this;
	}

	arbit& arbit::operator=(const arbit& rhs)
	{
		if (fixed_ptr) {
			free(fixed_ptr);
			fixed_ptr = nullptr;
		}
		if (decimal_ptr) {
			free(decimal_ptr);
			decimal_ptr = nullptr;
		}

		precision = rhs.precision;
		fixed_len = decimal_len = 0;
		actual_fixed_len = actual_decimal_len = 0;

		grow(rhs.fixed_len);

		if (fixed_len > 0)
			memcpy(fixed_ptr, rhs.fixed_ptr, sizeof(base_t) * fixed_len);
		if (decimal_len > 0)
			memcpy(decimal_ptr, rhs.decimal_ptr, sizeof(base_t) * decimal_len);

		return *this;
	}

	void arbit::raw_print(char way, bool newline) const
	{
		std::print("({},{})", actual_fixed_len, fixed_len);
		if (fixed_len > 0)
		{
			std::print(" ");
			if (is_negative()) std::print("!");
			for (unsigned i=0; i < fixed_len; i++)
			{
				auto unit = fixed_ptr[i];
				if (way == 'b')
					std::print("{:#b}", unit);
				else if (way == 'x')
					std::print("{:#x}", unit);
				else if (way == 's')
					std::print("{}", sbase_t(unit));
				else
					std::print("{}", unit);
				if (i != fixed_len-1)
					std::print(" ");
			}
			if (newline)
				std::println("");
		}

		if (decimal_len > 0)
		{
			WC_STD_EXCEPTION("{}:{}: Decimal printing unimplemented", __FILE__, __LINE__);
		}
	}

	void arbit::print() const
	{
		/*
		std::string digits;

		const bool neg = is_negative();

		{
			arbit copy;
			const arbit* source = this;
			if (neg)
			{
				source = &copy;
				copy = *this;
				copy.negate();
			}

			for (size_t i=0; i < source->fixed_len; i++)
			{
				base_t n = source->fixed_ptr[i];
				while (n != 0)
				{
					digits += std::to_string(n % 10);
					n /= 10;
				}
			}
		}

		if (digits.empty()) digits += "0";
		std::reverse(digits.begin(), digits.end());

		if (neg) std::print("-");
		std::print("{}", digits);

		if (decimal_len > 0)
			WC_STD_EXCEPTION("{}:{}: Decimal printing is broken", __FILE__, __LINE__);
		*/
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
		arbit multiplier(1);

		for (auto it = fixed.rbegin(); it != fixed.rend(); it++)
		{
			arbit cur((*it) - '0');
			cur *= multiplier;
			multiplier *= 10;

			*this += cur;
		}

		if (neg)
			negate();

		if (decimal.size() > 0)
		{
			WC_STD_EXCEPTION("{}:{}: Decimal parsing is broken", __FILE__, __LINE__);
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
			memset(&fixed_ptr[fixed_len], neg ? 0xff : 0, sizeof(base_t) * by);
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

			memset(&fixed_ptr[actual_fixed_len], neg ? 0xff : 0, sizeof(base_t) * by);

			actual_fixed_len = new_actual_fixed_len;
			fixed_len = new_fixed_len;
		}
	}

	void arbit::shrink(size_t by)
	{
		if (by > fixed_len)
			WC_STD_EXCEPTION("Cannot shrink by {} when {} is all it has", by, fixed_len);

		const size_t new_actual_fixed_len = actual_fixed_len;
		const size_t new_fixed_len = fixed_len - by;

		/*
		if(!(fixed_ptr = (base_t*)realloc((void*)fixed_ptr, sizeof(base_t) * new_actual_fixed_len)))
			WC_STD_EXCEPTION("Failed to reallocate from length {} to {}",
							 actual_fixed_len, new_actual_fixed_len);
		*/

		actual_fixed_len = new_actual_fixed_len;
		fixed_len = new_fixed_len;
	}
};
