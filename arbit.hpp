#pragma once

#include <cstdint>
#include <list>
#include <print>
#include <limits>
#include <cstdlib>

#include "utility.hpp"

namespace wc
{
	class arbit
	{
	public:
		using base_t = uint32_t;

	private:
		uint8_t precision;
		base_t *fixed_ptr, *decimal_ptr=nullptr;
		size_t fixed_len, decimal_len=0;

	public:
		arbit(const arbit& other) = delete;
		arbit(arbit&& other);
		arbit(base_t fixed=0, base_t decimal=0, uint8_t precision=50);
		~arbit();

		arbit& operator+=(const arbit& rhs);

		void raw_print();
	};
};
