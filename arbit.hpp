#pragma once

#include <cstdint>
#include <cstring>
#include <cmath>
#include <list>
#include <print>
#include <limits>
#include <string_view>
#include <cstdlib>
#include <cctype>
#include <exception>
#include <format>
#include <array>
#include <string>
#include <algorithm>
#include <initializer_list>
#include <unordered_map>

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
		using base_t = uint32_t;
		using sbase_t = int32_t;
		using base_double_t = uint64_t;

		class _stats {
			friend class wc::arbit;
		private:
			struct {
				size_t copy = 0, move = 0, parse = 0;
				size_t bare = 0, list = 0;
			} cons;

			struct {
				std::unordered_map<void*, size_t> allocs;
				size_t max = 0, max_entries = 0, current = 0;
				size_t mallocs = 0, reallocs = 0, frees = 0;
			} heap;

		public:
			_stats() {}
			const auto& get_cons() { return cons; }
			const auto& get_heap() { return heap; }
		} static inline stats;

		static const size_t base_bits = sizeof(base_t) * 8;
		static const base_t base_max = ~base_t(0), base_zero = 0;

		enum class error_type { parse };
		static constexpr std::array<std::string_view, 1> error_type_str { "parse" };
		class exception;

	private: // essential member variables
		base_t *fixed_ptr = nullptr, *decimal_ptr = nullptr;
		size_t fixed_len = 0, decimal_len = 0;
		size_t actual_fixed_len = 0, actual_decimal_len = 0;

	private: // helper functions
		void parse(std::string_view both);
		void parse(std::string_view fixed, std::string_view decimal, bool neg);

		void grow(size_t by);
		void grow(size_t by, bool neg);
		void shrink(size_t by);
		void grow_decimal(size_t by);
		void grow_decimal(size_t by, bool neg);
		void shrink_decimal(size_t by);

		arbit multiply(const arbit& rhs) const;
		arbit multiply(const arbit& rhs);
		arbit multiply(sbase_t rhs) const;
		arbit multiply(sbase_t rhs);
		static arbit multiply_raw(arbit& lhs, const arbit& rhs);
		static arbit multiply_raw(arbit& lhs, sbase_t rhs);

		template<class T> static void is_valid_integer();

	private: // heap management
		static void* internal_malloc(size_t size);
		static void* internal_realloc(void* ptr, size_t new_size);
		static void internal_free(void* ptr);

	public: // construction
		arbit(const arbit& other);
		arbit(arbit&& other);
		arbit(base_t fixed);
		arbit(base_t fixed, base_t decimal);
		arbit(std::string_view both);
		arbit(const base_t* fixed_ptr, size_t fixed_len, const base_t* decimal_ptr, size_t decimal_len);
		template<class It> arbit(It fixed_begin, size_t fixed_len, It decimal_begin, size_t decimal_len);

		~arbit();
		void reset();

	public: // bit manipulation
		static bool bit_raw(size_t at, size_t len, base_t* ptr, bool invert);
		static void clear_bit_raw(size_t at, size_t len, base_t* ptr, bool invert);
		static void set_bit_raw(size_t at, size_t len, base_t* ptr, bool invert);
		static void flip_bit_raw(size_t at, size_t len, base_t* ptr, bool invert);
		static void clear_first_bits_raw(size_t before, size_t len, base_t* ptr, bool invert);
		bool bit(size_t at) const;
		void clear_bit(size_t at);
		void set_bit(size_t at);
		void flip_bit(size_t at);
		void clear_first_bits(size_t before);
		bool bit_decimal(size_t at) const;
		void clear_decimal_bit(size_t at);
		void set_decimal_bit(size_t at);
		void flip_decimal_bit(size_t at);
		void clear_decimal_first_bits(size_t before);
		bool bit_both(size_t at) const;
		void clear_both_bit(size_t at);
		void set_both_bit(size_t at);
		void clear_both_first_bits(size_t before);

	public: // basic
		void zero();
		bool is_zero() const;
		bool is_negative() const;
		static bool is_negative(base_t n);

		void shrink_if_can_raw(bool fixed_not_decimal);
		void shrink_if_can();
		size_t bytes() const;
		size_t bytes_decimal() const;
		size_t bytes_total() const;

		std::string raw_format(char way) const;
		std::string format() const;

	public: // operations
		arbit& negate();
		arbit operator-() const { arbit copy(*this); copy.negate(); return copy; }
		arbit& operator-=(const arbit& rhs) { *this += -rhs; return *this; }
		arbit operator-(const arbit& rhs) const { arbit copy(*this); copy += -rhs; return copy; }
		arbit& operator+=(const arbit& rhs);
		arbit operator+(const arbit& rhs) const { arbit copy(*this); copy += rhs; return copy; }
		arbit& operator*=(const arbit& rhs);
		arbit operator*(const arbit& rhs) const;

		arbit& operator-=(sbase_t rhs) { *this += -rhs; return *this; }
		arbit operator-(sbase_t rhs) const { arbit copy(*this); copy += -rhs; return copy; }
		arbit& operator+=(sbase_t rhs);
		arbit operator+(sbase_t rhs) const { arbit copy(*this); copy += rhs; return copy; }
		arbit& operator*=(sbase_t rhs);
		arbit operator*(sbase_t rhs) const;

		arbit& operator<<=(size_t by);
		arbit& operator=(const arbit& rhs);
	};

	class arbit::exception : public std::exception
	{
	public:
		arbit::error_type type;
		std::string msg;

		exception(std::string_view msg, arbit::error_type type)
			:type(type), msg(msg)
		{}

		const char* what() const noexcept override
		{ return msg.c_str(); }
	};
};

#include "arbit.inl"
