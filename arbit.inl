namespace wc
{
	template<typename T> arbit& arbit::operator+=(T rhs)
	{
		is_valid_integer<T>();

		if (fixed_len == 0)
			grow(1);

		size_t where = 0;
		while (rhs != 0)
		{
			base_t unit_rhs = rhs & ~base_t(0);
			rhs >>= sizeof(base_t) * 8;

			do
			{
				base_t* unit_lhs_ptr = &fixed_ptr[where];

				if (base_max - *unit_lhs_ptr < unit_rhs) // overflow!
				{
					const auto left = unit_rhs - (base_max - *unit_lhs_ptr);
					*unit_lhs_ptr = base_max;
					unit_rhs = left;

					if (where >= fixed_len-1)
						grow(1);
					where++;
				}
				else
				{
					*unit_lhs_ptr += unit_rhs;
					unit_rhs = 0;
				}
			} while (unit_rhs != 0);
		}

		return *this;
	}
};

