#pragma once

#define ARBIT_EXCEPTION(type, ...) {									\
		auto sl = std::source_location::current();						\
		auto msg = std::format("{}:{} in {}: ", sl.file_name(), sl.line(), sl.function_name()); \
		msg += std::format(__VA_ARGS__);								\
		throw arbit::exception(msg, arbit::error_type::type);			\
	}
