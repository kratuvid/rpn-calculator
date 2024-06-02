#include "arbit.hpp"

namespace wc
{
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

	arbit::arbit(std::string_view both)
	{
		stats.cons.parse++;
		parse(both);
	}

	arbit::arbit(arbit::base_t fixed)
		:arbit(&fixed, 1, nullptr, 0)
	{
		stats.cons.bare++;
	}

	arbit::arbit(arbit::base_t fixed, arbit::base_t decimal)
		:arbit(&fixed, 1, &decimal, 1)
	{
		stats.cons.bare++;
	}

	arbit::arbit(const arbit::base_t* fixed_ptr, size_t fixed_len, const arbit::base_t* decimal_ptr, size_t decimal_len)
	{
		stats.cons.list++;

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
};
