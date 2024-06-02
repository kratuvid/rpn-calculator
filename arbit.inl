namespace wc
{
	template<class It>
	arbit::arbit(const std::pair<It, It>& fixed_it, size_t fixed_len, const std::pair<It, It>& decimal_it, size_t decimal_len, size_t precision)
		:precision(precision)
	{
		if (fixed_len > 0)
		{
			grow(fixed_len);

			size_t i=0;
			for (auto it = fixed_it.first; it != fixed_it.second; it++, i++)
				fixed_ptr[i] = *it;
		}

		if (decimal_len > 0)
		{
			const auto take = decimal_len > precision ? precision : decimal_len;
			if (take > 0)
			{
				grow_decimal(take);

				size_t i=0;
				for (auto it = decimal_it.first; i < take; it++, i++)
					decimal_ptr[i] = *it;
			}
		}
	}

	template<class T>
	void arbit::is_valid_integer()
	{
		static_assert(std::numeric_limits<T>::is_integer);
		static_assert(std::numeric_limits<T>::is_signed);
	}
};
