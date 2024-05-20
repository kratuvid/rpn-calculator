#pragma once

#include <exception>
#include <string_view>
#include <array>

namespace sc
{
	enum class error_type
	{
		init, init_help,
		expr,
		eval,
		repl_quit
	};

	static constexpr std::array<std::string_view, 5> error_type_str
	{
		"init", "init_help",
		"expr",
		"eval",
		"repl_quit"
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
