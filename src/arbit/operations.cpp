#include "arbit/arbit.hpp"
#include <cstring>

namespace wc
{
	arbit& arbit::negate()
	{
		for (size_t i=0; i < fixed_len; i++)
			fixed_ptr[i] = ~fixed_ptr[i];
		for (size_t i=0; i < decimal_len; i++)
			decimal_ptr[i] = ~decimal_ptr[i];

		base_double_t carry = 0;

		for (ssize_t i = decimal_len-1; i >= 0; i--)
		{
			const auto unit = decimal_ptr[i];
			const auto unit_rhs = i == (ssize_t)decimal_len-1 ? 1 : 0;
			const base_double_t sum = base_double_t(unit) + base_double_t(unit_rhs) + carry;
			carry = sum >> base_bits;
			decimal_ptr[i] = sum;
		}

		carry = decimal_len == 0 ? 1 : carry;
		if (carry > 0)
			*this += sbase_t(carry);

		return *this;
	}

	arbit& arbit::operator+=(const arbit& rhs)
	{
		if (rhs.fixed_len == 0)
			return *this;

		const auto neg = is_negative(), neg_rhs = rhs.is_negative();

		if (fixed_len < rhs.fixed_len)
		{
			const size_t by = rhs.fixed_len - fixed_len;
			grow(by);
		}

		if (decimal_len < rhs.decimal_len)
		{
			const size_t by = rhs.decimal_len - decimal_len;
			grow_decimal(by);
		}

		base_double_t carry = 0;

		for (ssize_t i = decimal_len-1; i >= 0; i--)
		{
			const base_t unit = decimal_ptr[i];
			const base_t unit_rhs = i < (ssize_t)rhs.decimal_len ? rhs.decimal_ptr[i] : 0;

			const base_double_t sum = base_double_t(unit) + base_double_t(unit_rhs) + carry;
			carry = sum >> base_bits;

			decimal_ptr[i] = sum;
		}

		for (size_t i=0; i < fixed_len; i++)
		{
			const base_t unit = fixed_ptr[i];
			const base_t unit_rhs = i < rhs.fixed_len ? rhs.fixed_ptr[i] : 0;
			
			const base_double_t sum = base_double_t(unit) + base_double_t(unit_rhs) + carry;
			carry = sum >> base_bits;

			fixed_ptr[i] = sum;

			if (i == fixed_len-1)
			{
				if (neg && neg_rhs && !is_negative(sum))
					grow(1, true);
				else if (!neg && !neg_rhs && is_negative(sum))
					grow(1, false);
				break;
			}
		}

		return *this;
	}

	arbit& arbit::operator*=(const arbit& rhs)
	{
		auto product = multiply(rhs);
		*this = std::move(product);
		return *this;
	}

	arbit arbit::operator*(const arbit& rhs) const
	{
		return multiply(rhs);
	}

	arbit& arbit::operator+=(arbit::sbase_t rhs)
	{
		if (fixed_len == 0)
			grow(1);

		const auto neg = is_negative(), neg_rhs = rhs < 0;

		base_double_t carry = 0;

		for (size_t i=0; i < fixed_len; i++)
		{
			const base_t unit = fixed_ptr[i];
			const base_t unit_rhs = i == 0 ? rhs : 0;
			
			const base_double_t sum = base_double_t(unit) + base_double_t(unit_rhs) + carry;
			carry = sum >> base_bits;

			fixed_ptr[i] = sum;

			if (i == fixed_len-1)
			{
				if (neg && neg_rhs && !is_negative(sum))
					grow(1, true);
				else if (!neg && !neg_rhs && is_negative(sum))
					grow(1, false);
				break;
			}
		}

		return *this;
	}

	arbit& arbit::operator*=(arbit::sbase_t rhs)
	{
		auto product = multiply(rhs);
		*this = std::move(product);
		return *this;
	}

	arbit arbit::operator*(arbit::sbase_t rhs) const
	{
		return multiply(rhs);
	}

	arbit& arbit::operator<<=(size_t by)
	{
		const auto bits_total = bytes_total() * 8;
		if (by >= bits_total)
		{
			if (fixed_len > 0)
				memset(fixed_ptr, 0, bytes());
			if (decimal_len > 0)
				memset(decimal_ptr, 0, bytes_decimal());
		}
		else if (by > 0)
		{
			for (ssize_t i = (ssize_t)bits_total-1; i >= 0; i--)
			{
				if ((size_t)i >= by)
				{
					const size_t at = (size_t)i - by;
					if (bit_both(at)) set_both_bit(i);
					else clear_both_bit(i);
				}
				else break;
			}
			const auto upto = by;
			clear_both_first_bits(upto);
		}

		return *this;
	}

	arbit& arbit::operator=(const arbit& rhs)
	{
		if (fixed_ptr) {
			internal_free(fixed_ptr);
			fixed_ptr = nullptr;
		}
		if (decimal_ptr) {
			internal_free(decimal_ptr);
			decimal_ptr = nullptr;
		}

		fixed_len = decimal_len = 0;
		actual_fixed_len = actual_decimal_len = 0;

		grow(rhs.fixed_len);
		grow_decimal(rhs.decimal_len);

		if (fixed_len > 0)
			memcpy(fixed_ptr, rhs.fixed_ptr, sizeof(base_t) * fixed_len);
		if (decimal_len > 0)
			memcpy(decimal_ptr, rhs.decimal_ptr, sizeof(base_t) * decimal_len);

		return *this;
	}

	arbit arbit::multiply(const arbit& rhs) const
	{
		auto copy(*this);
		return multiply_raw(copy, rhs);
	}

	arbit arbit::multiply(const arbit& rhs)
	{
		return multiply_raw(*this, rhs);
	}
	
	arbit arbit::multiply(sbase_t rhs) const
	{
		auto copy(*this);
		return multiply_raw(copy, rhs);
	}

	arbit arbit::multiply(sbase_t rhs)
	{
		return multiply_raw(*this, rhs);
	}

	arbit arbit::multiply_raw(arbit& lhs, const arbit& rhs)
	{
		arbit product(0u);

		if (lhs.fixed_len == 0 || rhs.fixed_len == 0)
			return product;

		arbit& copy = lhs;
		arbit copy_rhs(rhs);

		const auto total_decimal_len = copy.decimal_len + copy_rhs.decimal_len;
		const auto total_copy_len = copy.fixed_len + copy.decimal_len, total_copy_rhs_len = copy_rhs.fixed_len + copy_rhs.decimal_len;

		const size_t total_len = std::max(total_copy_len, total_copy_rhs_len) * 2;
		if (product.fixed_len < total_len)
			product.grow(total_len - product.fixed_len);
		if (total_copy_len < total_len)
			copy.grow(total_len - total_copy_len);
		if (total_copy_rhs_len < total_len)
			copy_rhs.grow(total_len - total_copy_rhs_len);

		const auto bits_total = copy_rhs.bytes_total() * 8;
		for (size_t i=0; i < bits_total; i++)
		{
			if (copy_rhs.bit_both(i))
				product += copy;
			copy <<= 1;
		}

		if ((product.decimal_len + product.fixed_len) > total_len)
			product.shrink((product.decimal_len + product.fixed_len) - total_len);

		product.shift_right_units(total_decimal_len - copy.decimal_len);

		product.shrink_if_can();
		return product;
	}

	arbit arbit::multiply_raw(arbit& lhs, sbase_t rhs)
	{
		arbit product(0u);

		if (lhs.fixed_len == 0 || rhs == 0)
			return product;

		const auto neg_rhs = rhs < 0;

		arbit& copy = lhs;

		const auto total_copy_len = copy.fixed_len + copy.decimal_len;

		const size_t total_len = std::max(total_copy_len, (size_t)1) * 2;
		if (product.fixed_len < total_len)
			product.grow(total_len - product.fixed_len);
		if (total_copy_len < total_len)
			copy.grow(total_len - total_copy_len);

		const auto bits = total_len * base_bits;
		for (size_t i=0; i < bits; i++)
		{
			const bool bit = i < sizeof(sbase_t) * 8 ? (rhs >> i) & 0x1 : neg_rhs;
			if (bit)
				product += copy;
			copy <<= 1;
		}

		if ((product.decimal_len + product.fixed_len) > total_len)
			product.shrink((product.decimal_len + product.fixed_len) - total_len);

		product.shrink_if_can();
		return product;
	}
};
