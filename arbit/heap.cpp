#include "arbit.hpp"
#include <cstring>

namespace wc
{
	void arbit::grow(size_t by)
	{
		grow(by, is_negative());
	}

	void arbit::grow(size_t by, bool neg)
	{
		if (by == 0) return;

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
		if (by == 0) return;

		const auto has_len = actual_decimal_len - decimal_len;
		if (by <= has_len)
		{
			memset(&decimal_ptr[decimal_len], 0, sizeof(base_t) * by);
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

			memset(&decimal_ptr[decimal_len], 0, sizeof(base_t) * by);

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
