#pragma once

#include "wc/defines.hpp"

import <array>;
import <exception>;
import <format>;
import <string_view>;

namespace wc
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

	class exception : public std::exception
	{
	public:
		error_type type;
		std::string msg;

		exception(std::string_view msg, error_type type)
			:type(type), msg(msg)
		{}

		const char* what() const noexcept override
		{ return msg.c_str(); }
	};
}; // namespace wc
