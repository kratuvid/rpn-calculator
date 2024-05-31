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
		if (fixed_len > 0)
			fixed_ptr[fixed_len-1] ^= 1 << (base_bits - 1);
		return *this;
	}

	arbit& arbit::operator+=(const arbit& rhs)
	{
		if (rhs.fixed_len == 0)
			return *this;

		const bool neg = is_negative(), neg_rhs = rhs.is_negative();

		if (fixed_len < rhs.fixed_len)
		{
			const size_t by = rhs.fixed_len - fixed_len;
			grow(by);
		}

		base_double_t carry = 0;
		bool rhs_done = false;

		for (size_t where=0; where < fixed_len; where++)
		{
			base_t unit_rhs = neg_rhs ? base_max : 0;
			if (!rhs_done)
			{
				unit_rhs = rhs.fixed_ptr[where];
				if (where == rhs.fixed_len-1)
					rhs_done = true;
			}

			base_t* unit_ptr = &fixed_ptr[where];
			base_double_t sum = base_double_t(*unit_ptr) + base_double_t(unit_rhs) + carry;

			carry = (sum >> base_bits) > 0 ? 1 : 0;
			*unit_ptr = sum & base_max;

			if (where == fixed_len-1)
			{
				if (neg && neg_rhs && !is_base_t_negative(*unit_ptr))
					grow(1, true);
				else if(!neg && !neg_rhs && is_base_t_negative(*unit_ptr))
					grow(1, false);
				break;
			}
		}

		return *this;
	}

	arbit arbit::operator*(const arbit& rhs)
	{
		arbit product, copy(*this), copy_rhs(rhs);

		const auto total_len = copy.fixed_len + copy_rhs.fixed_len;
		copy.grow(rhs.fixed_len);
		copy_rhs.grow(this->fixed_len);

		for (size_t i=0; i < copy_rhs.bytes() * 8; i++)
		{
			if (i != 0)
				copy <<= 1;
			if (copy_rhs.get_bit(i))
				product += copy;
		}

		if (product.fixed_len > total_len)
			product.shrink(product.fixed_len - total_len);

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
		std::print("Fixed: {},{}", actual_fixed_len, fixed_len);
		if (fixed_len > 0)
		{
			std::print("> ");
			if (is_negative()) std::print("!");
			for (unsigned i=0; i < fixed_len; i++)
			{
				auto unit = fixed_ptr[i];
				if (way == 'b')
					std::print("{:#b}", unit);
				else if (way == 'x')
					std::print("{:#x}", unit);
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
			WC_STD_EXCEPTION("{}:{}: decimal printing unimplemented", __FILE__, __LINE__);
		}
	}

	void arbit::print() const
	{
		WC_STD_EXCEPTION("{}:{}: print is broken", __FILE__, __LINE__);

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
			fixed_ptr[fixed_len-1] &= ~(1 << (base_bits - 1));
			memset(&fixed_ptr[fixed_len], 0, sizeof(base_t) * by);

			fixed_len += by;
			fixed_ptr[fixed_len-1] |= base_t(neg) << (base_bits - 1);
		}
		else
		{
			fixed_ptr[fixed_len-1] &= ~(1 << (base_bits - 1));

			const size_t grow_const = 1, grow_upper_limit = 1000;
			const auto new_actual_fixed_len = actual_fixed_len + by + grow_const;
			const auto new_fixed_len = fixed_len + by;

			if (new_actual_fixed_len > grow_upper_limit)
				WC_STD_EXCEPTION("Cannot grow to {} as {} is the upper limit",
								 new_actual_fixed_len, grow_upper_limit);

			if (!(fixed_ptr = (base_t*)realloc(fixed_ptr, sizeof(base_t) * new_actual_fixed_len)))
				WC_STD_EXCEPTION("Failed to reallocate from length {} to {}",
								 actual_fixed_len, new_actual_fixed_len);

			memset(&fixed_ptr[actual_fixed_len], 0, sizeof(base_t) * by);

			actual_fixed_len = new_actual_fixed_len;
			fixed_len = new_fixed_len;

			fixed_ptr[fixed_len-1] |= base_t(neg) << (base_bits - 1);
		}
	}

	void arbit::shrink(size_t by)
	{
		if (by >= fixed_len)
			WC_STD_EXCEPTION("Cannot shrink by {} when {} is all it has", by, fixed_len);

		const bool neg = fixed_ptr[fixed_len-1] >> (base_bits - 1);

		const size_t new_actual_fixed_len = actual_fixed_len;
		const size_t new_fixed_len = fixed_len - by;

		/*
		if(!(fixed_ptr = (base_t*)realloc((void*)fixed_ptr, sizeof(base_t) * new_actual_fixed_len)))
			WC_STD_EXCEPTION("Failed to reallocate from length {} to {}",
							 actual_fixed_len, new_actual_fixed_len);
		*/

		actual_fixed_len = new_actual_fixed_len;
		fixed_len = new_fixed_len;

		fixed_ptr[fixed_len-1] |= base_t(neg) << (base_bits - 1);
	}
};
