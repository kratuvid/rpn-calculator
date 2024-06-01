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
			WC_STD_EXCEPTION("{}:{}: Decimal unimplemented", __FILE__, __LINE__);
		}
	}

	template<class T>
	void arbit::is_valid_integer()
	{
		static_assert(std::numeric_limits<T>::is_integer);
		static_assert(std::numeric_limits<T>::is_signed);
	}
};
