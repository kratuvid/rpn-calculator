namespace wc
{
	template<typename T> arbit& arbit::operator-=(T rhs)
	{
		WC_STD_EXCEPTION("-= is broken");

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

	template<typename T> arbit& arbit::operator+=(T rhs)
	{
		// sizes greater than base_t are expected to be multiples of sizeof(base_t)

		is_valid_integer<T>();

		if (fixed_len == 0)
			grow(1);

		const bool neg = is_negative(), neg_rhs = rhs < 0;

		if (bytes() < sizeof(T))
		{
			const size_t by = std::ceil((sizeof(T) - bytes()) / double(sizeof(base_t)));
			grow(by);
		}

		base_double_t carry = 0;
		bool rhs_done = false;

		for (size_t where=0; where < fixed_len; where++)
		{
			base_t unit_rhs = neg_rhs ? base_max : 0;
			if (!rhs_done)
			{
				unit_rhs = rhs & base_max;
				base_t* rhs_indirect = (base_t*)&rhs;
				*rhs_indirect >>= sizeof(base_t) * 8;
				if (rhs == 0)
				{
					rhs_done = true;
					if (sizeof(T) < sizeof(base_t))
					{
						const auto diff_bits = (sizeof(base_t) - sizeof(T)) * 8;
						const auto ddiff_bits = (sizeof(base_t) * 8) - diff_bits;
						const base_t set_mask = ((neg_rhs ? base_max: 0) >> ddiff_bits) << ddiff_bits;
						unit_rhs |= set_mask;
					}
				}
			}

			base_t* unit_ptr = &fixed_ptr[where];
			base_double_t sum = base_double_t(*unit_ptr) + base_double_t(unit_rhs) + carry;

			*unit_ptr = sum & base_max;
			carry = (sum >> (sizeof(base_t) * 8)) > 0 ? 1 : 0;

			if (where == fixed_len-1)
			{
				if (neg && neg_rhs && !is_base_t_negative(*unit_ptr))
					grow(1, true);
				else if(!neg && !neg_rhs && is_base_t_negative(*unit_ptr))
					grow(1, false);
			}
		}

		return *this;
	}
};

