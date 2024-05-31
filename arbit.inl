namespace wc
{
	template<class C>
	arbit::arbit(const C& fixed, const C& decimal, base_t precision)
		:precision(precision)
	{
		if (fixed.size() > 0)
		{
			const bool neg = is_negative(*std::next(fixed.end(), -1));
			grow(fixed.size(), neg);

			size_t i=0;
			for (auto it = fixed.begin(); it != fixed.end(); it++, i++)
				fixed_ptr[i] = *it;
		}
		else grow(1);

		if (decimal.size() > 0)
		{
			WC_STD_EXCEPTION("{}:{}: Decimal unimplemented", __FILE__, __LINE__);
		}
	}

	template<class T>
	void arbit::is_valid_integer()
	{
		static_assert(std::numeric_limits<T>::is_integer);
	}

	template<class T>
	T arbit::to_signmag(T n)
	{
		is_valid_integer<T>();
		if (std::numeric_limits<T>::is_signed)
		{
			if (n < 0)
			{
				n = -n;
				if (n < 0)
					n = 0;
				n |= 1 << (base_bits - 1);
			}
		}
		return n;
	}

	template<class It>
	void arbit::to_signmag(It first, It last)
	{
		for (auto it = first; it != last; it++)
			*it = to_signmag(*it);
	}
};
