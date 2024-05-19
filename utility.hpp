#pragma once

#include <exception>
#include <string_view>
#include <array>

namespace sc
{
	enum class error_type
	{
		init, init_help,
		expr, expr_divide_by_zero
	};

	static constexpr std::array<std::string_view, 4> error_type_str
	{
		"init", "init_help",
		"expr", "expr_divide_by_zero"
	};

	class exception : public std::runtime_error
	{
	public:
		error_type type;
	
		exception(const std::string_view& msg, error_type e)
			:std::runtime_error(msg.data()), type(e)
		{
		}
	};
};
