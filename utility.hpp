#pragma once

#include <exception>
#include <string_view>
#include <array>
#include <format>

#define WC_EXCEPTION(type, ...) {						\
		auto msg = std::format(__VA_ARGS__);			\
		throw wc::exception(msg, wc::error_type::type);	\
	}

#define WC_STD_EXCEPTION(...) {					\
		auto msg = std::format(__VA_ARGS__);	\
		throw std::runtime_error(msg);			\
	}

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
