#pragma once

#define ARBIT_EXCEPTION(type, ...) {							\
		auto msg = std::format(__VA_ARGS__);					\
		throw arbit::exception(msg, arbit::error_type::type);	\
	}
