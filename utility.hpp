#pragma once

#include <exception>
#include <string_view>
#include <array>
#include <sstream>

#define SC_EXCEPTION(type, ...) {								\
		std::ostringstream oss;									\
		oss << __VA_ARGS__;										\
		throw sc::exception(oss.str(), sc::error_type::type);	\
	}

#define SC_STD_EXCEPTION(...) {					\
		std::ostringstream oss;					\
		oss << __VA_ARGS__;						\
		throw std::runtime_error(oss.str());	\
	}

namespace sc
{
	enum class error_type
	{
		init, init_help,
		parse,
		eval,
		exec,
		file,
		repl_quit
	};

	static constexpr std::array<std::string_view, 7> error_type_str
	{
		"init", "init_help",
		"parse",
		"eval",
		"exec",
		"file",
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
