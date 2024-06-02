#include "arbit.hpp"

namespace wc
{
	arbit::arbit(const arbit& other)
	{
		stats.cons.copy++;
		*this = other;
	}

	arbit::arbit(arbit&& other)
		:precision(other.precision),
		 fixed_len(other.fixed_len), decimal_len(other.decimal_len),
		 actual_fixed_len(other.actual_fixed_len), actual_decimal_len(other.actual_decimal_len)
	{
		stats.cons.move++;

		if (fixed_ptr) {
			internal_free(fixed_ptr);
			fixed_ptr = nullptr;
		}
		if (decimal_ptr) {
			internal_free(decimal_ptr);
			decimal_ptr = nullptr;
		}

		fixed_ptr = other.fixed_ptr;
		decimal_ptr = other.decimal_ptr;

		other.fixed_ptr = other.decimal_ptr = nullptr;
		other.fixed_len = other.decimal_len = 0;
		other.actual_fixed_len = other.actual_decimal_len = 0;
		other.precision = default_precision;
	}

	arbit::arbit(std::string_view both, size_t precision)
		:precision(precision)
	{
		stats.cons.parse++;
		parse(both);
	}

	arbit::arbit(base_t fixed, size_t precision)
		:arbit(std::initializer_list<base_t>({fixed}), {}, precision)
	{
		stats.cons.bare++;
	}

	arbit::arbit(base_t fixed, base_t decimal, size_t precision)
		:arbit(std::initializer_list<base_t>({fixed}), std::initializer_list<base_t>({decimal}), precision)
	{
		stats.cons.bare++;
	}

	arbit::~arbit()
	{
		if (fixed_ptr) internal_free(fixed_ptr);
		if (decimal_ptr) internal_free(decimal_ptr);
	}

	void arbit::reset()
	{
		if (fixed_ptr) {
			internal_free(fixed_ptr);
			fixed_ptr = nullptr;
		}
		if (decimal_ptr) {
			internal_free(decimal_ptr);
			decimal_ptr = nullptr;
		}

		precision = default_precision;
		fixed_len = decimal_len = 0;
		actual_fixed_len = actual_decimal_len = 0;
	}

	bool arbit::bit_raw(size_t at, size_t len, base_t* ptr)
	{
		const size_t unit = at / base_bits, unit_at = at % base_bits;
		if (unit <= len-1)
			return ptr[unit] & (1 << unit_at);
		return false;
	}

	void arbit::clear_bit_raw(size_t at, size_t len, base_t* ptr)
	{
		const size_t unit = at / base_bits, unit_at = at % base_bits;
		if (unit <= len-1)
			ptr[unit] &= ~(1 << unit_at);
	}

	void arbit::set_bit_raw(size_t at, size_t len, base_t* ptr)
	{
		const size_t unit = at / base_bits, unit_at = at % base_bits;
		if (unit <= len-1)
			ptr[unit] |= (1 << unit_at);
	}

	void arbit::flip_bit_raw(size_t at, size_t len, base_t* ptr)
	{
		const size_t unit = at / base_bits, unit_at = at % base_bits;
		if (unit <= len-1)
			ptr[unit] ^= (1 << unit_at);
	}

	bool arbit::bit(size_t at) const
	{
		return bit_raw(at, fixed_len, fixed_ptr);
	}

	void arbit::clear_bit(size_t at)
	{
		clear_bit_raw(at, fixed_len, fixed_ptr);
	}

	void arbit::set_bit(size_t at)
	{
		set_bit_raw(at, fixed_len, fixed_ptr);
	}

	void arbit::flip_bit(size_t at)
	{
		flip_bit_raw(at, fixed_len, fixed_ptr);
	}

	bool arbit::bit_decimal(size_t at) const
	{
		return bit_raw(at, decimal_len, decimal_ptr);
	}

	void arbit::clear_decimal_bit(size_t at)
	{
		clear_bit_raw(at, decimal_len, decimal_ptr);
	}

	void arbit::set_decimal_bit(size_t at)
	{
		set_bit_raw(at, decimal_len, decimal_ptr);
	}

	void arbit::flip_decimal_bit(size_t at)
	{
		flip_bit_raw(at, decimal_len, decimal_ptr);
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
		clear_first_bits_raw(before, fixed_len, fixed_ptr, true);
	}

	void arbit::zero()
	{
		if (fixed_len > 1)
			shrink(fixed_len - 1);
		else if (fixed_len == 0)
			grow(1);
		fixed_ptr[0] = 0;
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

	void arbit::shrink_if_can()
	{
		const auto neg = is_negative();
		const auto check = neg ? base_max : 0;

		ssize_t i = (ssize_t)fixed_len - 1;
		for (; i >= 1; i--)
		{
			if (fixed_ptr[i] == check)
			{
				if (is_negative(fixed_ptr[i-1]) == neg)
					continue;
				else break;
			} else break;
		}

		const ssize_t by = (ssize_t)fixed_len - 1 - i;
		if (by > 0)
			shrink(by);
	}

	size_t arbit::bytes() const
	{
		return fixed_len * sizeof(base_t);
	}

	size_t arbit::bytes_decimal() const
	{
		return decimal_len * sizeof(base_t);
	}

	size_t arbit::bytes_total() const
	{
		return bytes() + bytes_decimal();
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

		const auto neg = is_negative(), neg_rhs = rhs.is_negative();

		base_double_t carry = 0;
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
		const auto bits = bytes() * 8;
		if (by >= bits)
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
			internal_free(fixed_ptr);
			fixed_ptr = nullptr;
		}
		if (decimal_ptr) {
			internal_free(decimal_ptr);
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
		if (fixed_len > 0)
		{
			std::print("F({},{}) ", actual_fixed_len, fixed_len);
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
		}

		if (decimal_len > 0)
		{
			std::print(" D({},{}) ", actual_decimal_len, decimal_len);
			for (unsigned i=0; i < decimal_len; i++)
			{
				auto unit = decimal_ptr[i];
				if (way == 'b')
					std::print("{:#b}", unit);
				else if (way == 'x')
					std::print("{:#x}", unit);
				else if (way == 's')
					std::print("{}", sbase_t(unit));
				else
					std::print("{}", unit);
				if (i != decimal_len-1)
					std::print(" ");
			}
		}

		if (newline)
			std::println("");
	}

	void arbit::print() const
	{
		WC_STD_EXCEPTION("{}:{}: print() is broken", __FILE__, __LINE__);
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

		std::string_view fixed(both.begin() + (neg ? 1 : 0), fixed_end), decimal;
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

			if (!(fixed_ptr = (base_t*)internal_realloc(fixed_ptr, sizeof(base_t) * new_actual_fixed_len)))
				WC_STD_EXCEPTION("Failed to reallocate from length {} to {}",
								 actual_fixed_len, new_actual_fixed_len);

			memset(&fixed_ptr[fixed_len], neg ? 0xff : 0, sizeof(base_t) * by);

			actual_fixed_len = new_actual_fixed_len;
			fixed_len = new_fixed_len;
		}
	}

	void arbit::shrink(size_t by)
	{
		if (by >= fixed_len)
			WC_STD_EXCEPTION("Cannot shrink by {} when {} is all it has", by, fixed_len);

		const size_t new_actual_fixed_len = actual_fixed_len - by;
		const size_t new_fixed_len = fixed_len - by;

		if(!(fixed_ptr = (base_t*)internal_realloc(fixed_ptr, sizeof(base_t) * new_actual_fixed_len)))
			WC_STD_EXCEPTION("Failed to reallocate from length {} to {}",
							 actual_fixed_len, new_actual_fixed_len);

		actual_fixed_len = new_actual_fixed_len;
		fixed_len = new_fixed_len;
	}

	void arbit::grow_decimal(size_t by)
	{
		grow_decimal(by, is_negative());
	}

	void arbit::grow_decimal(size_t by, bool neg)
	{
		const auto has_len = actual_decimal_len - decimal_len;
		if (by <= has_len)
		{
			memset(&decimal_ptr[decimal_len], neg ? 0xff : 0, sizeof(base_t) * by);
			decimal_len += by;
		}
		else
		{
			const size_t grow_const = 1, grow_upper_limit = 1000;
			const auto new_actual_decimal_len = actual_decimal_len + by + grow_const;
			const auto new_decimal_len = decimal_len + by;

			if (new_actual_decimal_len > grow_upper_limit)
				WC_STD_EXCEPTION("Cannot grow decimal to {} as {} is the upper limit",
								 new_actual_decimal_len, grow_upper_limit);

			if (!(decimal_ptr = (base_t*)internal_realloc(decimal_ptr, sizeof(base_t) * new_actual_decimal_len)))
				WC_STD_EXCEPTION("Failed to reallocate from length {} to {}",
								 actual_decimal_len, new_actual_decimal_len);

			memset(&decimal_ptr[decimal_len], neg ? 0xff : 0, sizeof(base_t) * by);

			actual_decimal_len = new_actual_decimal_len;
			decimal_len = new_decimal_len;
		}
	}

	void arbit::shrink_decimal(size_t by)
	{
		if (by >= decimal_len)
			WC_STD_EXCEPTION("Cannot shrink decimal by {} when {} is all it has", by, decimal_len);

		const size_t new_actual_decimal_len = actual_decimal_len - by;
		const size_t new_decimal_len = decimal_len - by;

		if(!(decimal_ptr = (base_t*)internal_realloc(decimal_ptr, sizeof(base_t) * new_actual_decimal_len)))
			WC_STD_EXCEPTION("Failed to reallocate from length {} to {}",
							 actual_decimal_len, new_actual_decimal_len);

		actual_decimal_len = new_actual_decimal_len;
		decimal_len = new_decimal_len;
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
		arbit product(0);

		if (lhs.fixed_len == 0 || rhs.fixed_len == 0)
			return product;

		arbit& copy = lhs;
		arbit copy_rhs(rhs);

		const size_t total_len = std::max(lhs.fixed_len, rhs.fixed_len) * 2;
		if (product.fixed_len < total_len)
			product.grow(total_len - product.fixed_len);
		if (copy.fixed_len < total_len)
			copy.grow(total_len - copy.fixed_len);
		if (copy_rhs.fixed_len < total_len)
			copy_rhs.grow(total_len - copy_rhs.fixed_len);

		const auto bits = copy_rhs.bytes() * 8;
		for (size_t i=0; i < bits; i++)
		{
			if (copy_rhs.bit(i))
				product += copy;

			copy <<= 1;
		}

		if (product.fixed_len > total_len)
			product.shrink(product.fixed_len - total_len);

		product.shrink_if_can();
		return product;
	}

	arbit arbit::multiply_raw(arbit& lhs, sbase_t rhs)
	{
		arbit product(0);

		if (lhs.fixed_len == 0 || rhs == 0)
			return product;

		const auto neg_rhs = rhs < 0;

		arbit& copy = lhs;

		const size_t total_len = std::max(lhs.fixed_len, (size_t)1) * 2;
		if (product.fixed_len < total_len)
			product.grow(total_len - product.fixed_len);
		if (copy.fixed_len < total_len)
			copy.grow(total_len - copy.fixed_len);

		const auto bits = total_len * sizeof(base_t) * 8;
		for (size_t i=0; i < bits; i++)
		{
			bool bit = i < sizeof(sbase_t) * 8 ? (rhs >> i) & 0x1 : neg_rhs;
			if (bit)
				product += copy;

			copy <<= 1;
		}

		if (product.fixed_len > total_len)
			product.shrink(product.fixed_len - total_len);

		product.shrink_if_can();
		return product;
	}

	/* Heap */
	
	void* arbit::internal_malloc(size_t size)
	{
		auto ptr = malloc(size);
		if (!ptr)
			return nullptr;

		stats.heap.allocs[ptr] = size;

		stats.heap.mallocs++;
		stats.heap.current += size;
		if (stats.heap.current > stats.heap.max)
			stats.heap.max = stats.heap.current;
		if (stats.heap.allocs.size() > stats.heap.max_entries)
			stats.heap.max_entries = stats.heap.allocs.size();

		return ptr;
	}

	void* arbit::internal_realloc(void* ptr, size_t new_size)
	{
		auto new_ptr = realloc(ptr, new_size);
		if (!new_ptr)
			return nullptr;

		stats.heap.current += new_size;

		if (ptr == nullptr)
		{
			stats.heap.mallocs++;
			stats.heap.allocs[new_ptr] = new_size;
		}
		else
		{
			auto it = stats.heap.allocs.find(ptr);

			stats.heap.reallocs++;
			stats.heap.current -= it->second;

			if (ptr == new_ptr)
			{
				it->second = new_size;
			}
			else
			{
				stats.heap.allocs.erase(it);
				stats.heap.allocs[new_ptr] = new_size;
			}
		}

		if (stats.heap.current > stats.heap.max)
			stats.heap.max = stats.heap.current;
		if (stats.heap.allocs.size() > stats.heap.max_entries)
			stats.heap.max_entries = stats.heap.allocs.size();

		return new_ptr;
	}

	void arbit::internal_free(void* ptr)
	{
		free(ptr);

		auto it = stats.heap.allocs.find(ptr);
		if (it == stats.heap.allocs.end())
		{
			WC_STD_EXCEPTION("Free called on a non-existent heap pointer {}", (size_t)ptr);
		}
		else
		{
			stats.heap.frees++;
			stats.heap.current -= it->second;
			stats.heap.allocs.erase(it);
		}
	}
};
