#pragma once

#include <cstdint>
#include <cstring>
#include <list>
#include <print>
#include <limits>
#include <string_view>
#include <cstdlib>
#include <cctype>
#include <exception>
#include <format>
#include <array>

#include "utility.hpp"

#define WC_ARBIT_EXCEPTION(type, ...) {									\
		auto msg = std::format(__VA_ARGS__);							\
		throw wc::arbit::exception(msg, wc::arbit::error_type::type);	\
	}

namespace wc
{
	class arbit
	{
	public:
		enum class error_type { parse };
		static constexpr std::array<std::string_view, 1> error_type_str { "parse" };
		class exception;

		using base_t = uint32_t;

	private:
		static const base_t default_precision = 50, base_max = ~base_t(0);

		base_t precision = default_precision;
		base_t *fixed_ptr=nullptr, *decimal_ptr=nullptr;
		size_t fixed_len=0, decimal_len=0;
		size_t actual_fixed_len=0, actual_decimal_len=0;

	private:
		template<typename T> void is_valid_integer()
		{
			static_assert(std::numeric_limits<T>::is_integer);
			static_assert(!std::numeric_limits<T>::is_signed);
		}

		void parse(std::string_view both);
		void parse(std::string_view fixed, std::string_view decimal);

		void grow(size_t by);
		void shrink(size_t by);

	public:
		arbit(const arbit& other);
		arbit(arbit&& other);
		arbit(std::string_view both, base_t precision=default_precision);
		arbit(base_t fixed=0, base_t decimal=0, base_t precision=default_precision);
		~arbit();

		base_t get_precision();
		base_t set_precision(base_t precision);

		template<typename T> arbit& operator+=(T rhs);
		arbit& operator+=(const arbit& rhs);

		void raw_print();
	};

	class arbit::exception : public std::runtime_error
	{
	public:
		arbit::error_type type;

		exception(const std::string_view& what, arbit::error_type type)
			:std::runtime_error(what.data()), type(type)
		{}
	};
};

#include "arbit.inl"
