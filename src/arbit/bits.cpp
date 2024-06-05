module arbit;

namespace wc
{
	bool arbit::bit_raw(size_t at, size_t len, base_t* ptr, bool invert)
	{
		const size_t unit = invert ? len - 1 - (at / base_bits) : at / base_bits, unit_at = at % base_bits;
		if (unit <= len-1)
			return ptr[unit] & (1 << unit_at);
		return false;
	}

	void arbit::clear_bit_raw(size_t at, size_t len, base_t* ptr, bool invert)
	{
		const size_t unit = invert ? len - 1 - (at / base_bits) : at / base_bits, unit_at = at % base_bits;
		if (unit <= len-1)
			ptr[unit] &= ~(1 << unit_at);
	}

	void arbit::set_bit_raw(size_t at, size_t len, base_t* ptr, bool invert)
	{
		const size_t unit = invert ? len - 1 - (at / base_bits) : at / base_bits, unit_at = at % base_bits;
		if (unit <= len-1)
			ptr[unit] |= (1 << unit_at);
	}

	void arbit::flip_bit_raw(size_t at, size_t len, base_t* ptr, bool invert)
	{
		const size_t unit = invert ? len - 1 - (at / base_bits) : at / base_bits, unit_at = at % base_bits;
		if (unit <= len-1)
			ptr[unit] ^= (1 << unit_at);
	}

	bool arbit::bit(size_t at) const
	{
		return bit_raw(at, fixed_len, fixed_ptr, false);
	}

	void arbit::clear_bit(size_t at)
	{
		clear_bit_raw(at, fixed_len, fixed_ptr, false);
	}

	void arbit::set_bit(size_t at)
	{
		set_bit_raw(at, fixed_len, fixed_ptr, false);
	}

	void arbit::flip_bit(size_t at)
	{
		flip_bit_raw(at, fixed_len, fixed_ptr, false);
	}

	bool arbit::bit_decimal(size_t at) const
	{
		return bit_raw(at, decimal_len, decimal_ptr, true);
	}

	void arbit::clear_decimal_bit(size_t at)
	{
		clear_bit_raw(at, decimal_len, decimal_ptr, true);
	}

	void arbit::set_decimal_bit(size_t at)
	{
		set_bit_raw(at, decimal_len, decimal_ptr, true);
	}

	void arbit::flip_decimal_bit(size_t at)
	{
		flip_bit_raw(at, decimal_len, decimal_ptr, true);
	}

	void arbit::clear_first_bits_raw(size_t before, size_t len, base_t* ptr, bool invert)
	{
		const size_t unit = invert ? len - 1 - (before / base_bits) : before / base_bits,
				  unit_at = before % base_bits;
		const ssize_t end = invert ? len : -1, change = invert ? 1 : -1;
		ssize_t i = unit;
		while (i != end)
		{
			if ((size_t)i == unit)
			{
				ptr[i] >>= unit_at;
				ptr[i] <<= unit_at;
			}
			else
				ptr[i] = 0;

			i += change;
		}
	}

	void arbit::clear_first_bits(size_t before)
	{
		clear_first_bits_raw(before, fixed_len, fixed_ptr, false);
	}

	void arbit::clear_decimal_first_bits(size_t before)
	{
		clear_first_bits_raw(before, decimal_len, decimal_ptr, true);
	}

	void arbit::clear_both_first_bits(size_t before)
	{
		const auto bits_fixed = bytes() * 8, bits_decimal = bytes_decimal() * 8;
		const bool is_decimal_covered = before > 0 && bits_decimal != 0,
			  is_fixed_covered = before > bits_decimal && bits_fixed != 0;

		if (is_decimal_covered)
		{
			const size_t before_this = is_fixed_covered ? bits_decimal : before;
			clear_decimal_first_bits(before_this);
			before -= bits_decimal;
		}

		if (is_fixed_covered)
		{
			clear_first_bits(before);
		}
	}

	bool arbit::bit_both(size_t at) const
	{
		const auto bits_decimal = bytes_decimal() * 8;
		const bool is_decimal_covered = at >= 0 && at < bits_decimal && bits_decimal != 0;
		if (is_decimal_covered)
		{
			return bit_decimal(at);
		}
		else
		{
			at -= bits_decimal;
			return bit(at);
		}
	}

	void arbit::clear_both_bit(size_t at)
	{
		const auto bits_decimal = bytes_decimal() * 8;
		const bool is_decimal_covered = at >= 0 && at < bits_decimal && bits_decimal != 0;
		if (is_decimal_covered)
		{
			clear_decimal_bit(at);
		}
		else
		{
			at -= bits_decimal;
			clear_bit(at);
		}
	}

	void arbit::set_both_bit(size_t at)
	{
		const auto bits_decimal = bytes_decimal() * 8;
		const bool is_decimal_covered = at >= 0 && at < bits_decimal && bits_decimal != 0;
		if (is_decimal_covered)
		{
			set_decimal_bit(at);
		}
		else
		{
			at -= bits_decimal;
			set_bit(at);
		}
	}
};
