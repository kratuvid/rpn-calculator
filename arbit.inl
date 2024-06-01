namespace wc
{
	template<class C>
	arbit::arbit(const C& fixed, const C& decimal, base_t precision)
		:precision(precision)
	{
		static_assert(typeid(C) != typeid(std::string));
		static_assert(typeid(C) != typeid(std::string_view));
		static_assert(typeid(C) != typeid(const char*));
		static_assert(typeid(C) != typeid(char*));

		stats.cons.list++;

		if (fixed.size() > 0)
		{
			grow(fixed.size());

			size_t i=0;
			for (auto it = fixed.begin(); it != fixed.end(); it++, i++)
				fixed_ptr[i] = *it;
		}

		if (decimal.size() > 0)
		{
			grow_decimal(decimal.size());

			size_t i=0;
			for (auto it = decimal.begin(); it != decimal.end(); it++, i++)
				decimal_ptr[i] = *it;
		}
	}

	template<class T>
	void arbit::is_valid_integer()
	{
		static_assert(std::numeric_limits<T>::is_integer);
		static_assert(std::numeric_limits<T>::is_signed);
	}
};
