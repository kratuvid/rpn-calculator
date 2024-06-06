module arbit;

#include "arbit/defines.hpp"

arbit::arbit(const arbit& other)
{
	stats.cons.copy++;
	*this = other;
}

arbit::arbit(arbit&& other)
	:fixed_len(other.fixed_len), decimal_len(other.decimal_len),
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
}

arbit::arbit(float both) // expects base_t to be 32-bits long
{
	stats.cons.normal++;

	const auto category = std::fpclassify(both);
	if (!(category == FP_NORMAL || category == FP_SUBNORMAL || category == FP_ZERO))
		ARBIT_EXCEPTION(parse, "Only floating point categories normal, subnormal and zero are supported");

	const uint32_t* ff = (uint32_t*)&both;
	const uint32_t sign = *ff >> 31;
	const uint32_t exp = (*ff >> 23) & ~(uint32_t(1) << 8);
	const int32_t exp_norm = exp - 127;
	const uint32_t mantissa = *ff & ~(uint32_t(0x1ff) << 23);
		
	std::vector<base_t, std::allocator<base_t>> fixed, decimal;

	if (category == FP_NORMAL)
	{
		const uint32_t mantissa_real = mantissa | (uint32_t(1) << 23);

		ssize_t unit = exp_norm / 32, unit_at = exp_norm % 32;
		if (exp_norm < 0)
		{
			unit = -(((-exp_norm - 1) / 32) + 1);
			unit_at = 31 - ((-exp_norm - 1) % 32);
		}

		const ssize_t unit_prev = unit - 1, unit_bits_left = 24 - (unit_at + 1);

		if (unit >= 0)
		{
			for (ssize_t i=0; i < unit+1; i++)
				fixed.push_back(0);
			if (unit_at <= 23)
				fixed.back() = mantissa_real >> ((24 - 1) - unit_at);
			else
				fixed.back() = (mantissa_real << 8) >> ((32 - 1) - unit_at);
		}
		else
		{
			for (ssize_t i=0; i < -unit; i++)
				decimal.push_back(0);
			if (unit_at <= 23)
				decimal.back() = mantissa_real >> ((24 - 1) - unit_at);
			else
				decimal.back() = (mantissa_real << 8) >> ((32 - 1) - unit_at);
		}

		if (unit_bits_left > 0)
		{
			const auto mantissa_real_left = (mantissa_real << 8) << (24 - unit_bits_left);
			if (unit_prev >= 0)
				fixed[unit_prev] = mantissa_real_left;
			else
				decimal.push_back(mantissa_real_left);
		}

		if (fixed.size() == 0)
			fixed.push_back(0);
	}
	else if (category == FP_SUBNORMAL)
	{
		// Mantissa will lie on the last two bits of unit 4 (2^-96 to 2^-128)
		// Rest on the high bits of unit 5 (2^-129 to 2^-160)
		// 1 / 2^(32 * 5) = 6.84e-49
		// 2^-127 = 5.88e-39

		fixed.push_back(0);
		for (int i=0; i < 5; i++)
			decimal.push_back(0);

		decimal[3] = mantissa >> 21;
		decimal[4] = mantissa << 11;
	}
	else if (category == FP_ZERO)
	{
		fixed.push_back(0);
	}

	grow(fixed.size());
	size_t i = 0;
	for (auto it = fixed.begin(); it != fixed.end(); it++, i++)
		fixed_ptr[i] = *it;

	grow_decimal(decimal.size());
	i = 0;
	for (auto it = decimal.begin(); it != decimal.end(); it++, i++)
		decimal_ptr[i] = *it;

	if (sign)
		negate();
}

arbit::arbit(std::string_view both)
{
	stats.cons.normal++;
	parse(both);
}

arbit::arbit(arbit::base_t fixed)
	:arbit(&fixed, 1, nullptr, 0)
{
}

arbit::arbit(arbit::base_t fixed, arbit::base_t decimal)
	:arbit(&fixed, 1, &decimal, 1)
{
}

arbit::arbit(const arbit::base_t* fixed_ptr, size_t fixed_len, const arbit::base_t* decimal_ptr, size_t decimal_len)
{
	stats.cons.normal++;

	grow(fixed_len);
	for (size_t i=0; i < fixed_len; i++)
		this->fixed_ptr[i] = fixed_ptr[i];

	grow_decimal(decimal_len);
	for (size_t i=0; i < decimal_len; i++)
		this->decimal_ptr[i] = decimal_ptr[i];
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

	fixed_len = decimal_len = 0;
	actual_fixed_len = actual_decimal_len = 0;
}
