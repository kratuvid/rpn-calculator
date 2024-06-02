namespace wc
{
	template<class It>
	arbit::arbit(It fixed_begin, size_t fixed_len, It decimal_begin, size_t decimal_len)
	{
		stats.cons.list++;

		grow(fixed_len);
		size_t i = 0;
		for (auto it = fixed_begin; i < fixed_len; it++, i++)
			fixed_ptr[i] = *it;

		grow_decimal(decimal_len);
		i = 0;
		for (auto it = decimal_begin; i < decimal_len; it++, i++)
			decimal_ptr[i] = *it;
	}

	template<class T>
	void arbit::is_valid_integer()
	{
		static_assert(std::numeric_limits<T>::is_integer);
		static_assert(std::numeric_limits<T>::is_signed);
	}
};
