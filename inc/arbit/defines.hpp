#pragma once

#define WC_ARBIT_EXCEPTION(type, ...) {									\
		auto msg = std::format(__VA_ARGS__);							\
		throw wc::arbit::exception(msg, wc::arbit::error_type::type);	\
	}
