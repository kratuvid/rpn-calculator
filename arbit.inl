namespace wc
{
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
				*rhs_indirect >>= base_bits;
				if (rhs == 0)
				{
					rhs_done = true;
					if (sizeof(T) < sizeof(base_t))
					{
						const auto diff_bits = (sizeof(base_t) - sizeof(T)) * 8;
						const auto ddiff_bits = base_bits - diff_bits;
						const base_t set_mask = ((neg_rhs ? base_max: 0) >> ddiff_bits) << ddiff_bits;
						unit_rhs |= set_mask;
					}
				}
			}

			base_t* unit_ptr = &fixed_ptr[where];
			base_double_t sum = base_double_t(*unit_ptr) + base_double_t(unit_rhs) + carry;

			carry = (sum >> base_bits) > 0 ? 1 : 0;
			*unit_ptr = sum & base_max;

			if (where == fixed_len-1)
			{
				if (neg && neg_rhs && !is_base_t_negative(*unit_ptr))
					grow(1, true);
				else if(!neg && !neg_rhs && is_base_t_negative(*unit_ptr))
					grow(1, false);
				break;
			}
		}

		return *this;
	}

	template<typename T> arbit arbit::operator*(T rhs)
	{
		is_valid_integer<T>();

		arbit product;

		size_t i = 0;
		while (rhs != 0)
		{
			base_t* rhs_indirect = (base_t*)&rhs;
			base_t bit = *rhs_indirect & 0x1;
			*rhs_indirect >>= 1;

			

			i++;
		}

		return product;
	}
};
